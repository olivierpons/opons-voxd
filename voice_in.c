/*
 * voice_in.c -- System-tray push-to-talk dictation for Linux.
 *
 * Left-click the tray icon to toggle recording. Audio is captured via
 * PortAudio, transcribed with whisper.cpp, pushed to both X11 selections
 * (PRIMARY + CLIPBOARD) via xclip, and shown as a desktop notification
 * through libnotify.
 *
 * Build: see Makefile.
 * Runtime env:
 *     VOICE_IN_MODEL     path to ggml-*.bin (default: ./models/ggml-medium.bin)
 *     VOICE_IN_LANGUAGE  ISO code, or "auto" (default: fr)
 *     VOICE_IN_DEVICE    PortAudio device index (default: system default)
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <portaudio.h>

/* GtkStatusIcon has been deprecated since GTK 3.14 in favor of AppIndicator,
 * but AppIndicator has no primary-click activate signal (clicking always
 * opens the menu), which defeats the whole point of a one-click toggle. The
 * deprecated API works fine on Cinnamon/Mint, we silence the noise. */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "whisper.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPLE_RATE          16000
#define CHANNELS             1
#define FRAMES_PER_BUFFER    1024
#define MAX_RECORDING_SEC    600   /* 10 minutes max per session */
#define AUDIO_CAPACITY       ((size_t)SAMPLE_RATE * MAX_RECORDING_SEC)
#define ICON_SIZE            22
#define NOTIFY_TIMEOUT_MS    10000
#define DEFAULT_MODEL_PATH   "whisper.cpp/models/ggml-medium.bin"
#define DEFAULT_LANGUAGE     "fr"

typedef enum {
    STATE_IDLE,
    STATE_RECORDING,
    STATE_PROCESSING,
} app_state_t;

typedef struct {
    /* GTK */
    GtkStatusIcon *status_icon;
    GdkPixbuf     *icon_idle;
    GdkPixbuf     *icon_recording;
    GdkPixbuf     *icon_processing;
    GtkWidget     *menu;

    /* PortAudio */
    PaStream *stream;
    int       input_device;

    /* Audio buffer: float32 mono @ 16 kHz, pre-allocated */
    float         *audio_buffer;
    atomic_size_t  audio_size;      /* written by RT callback */

    /* Whisper */
    struct whisper_context *whisper_ctx;
    char                    language[16];

    /* State */
    _Atomic app_state_t state;
} app_t;

static app_t g_app;

/* ------------------------------------------------------------------------- */
/*                                  Icons                                    */
/* ------------------------------------------------------------------------- */

static GdkPixbuf *make_icon_pixbuf(double r, double g, double b, gboolean filled) {
    cairo_surface_t *surface =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ICON_SIZE, ICON_SIZE);
    cairo_t *cr = cairo_create(surface);

    const double cx = ICON_SIZE / 2.0;
    const double cy = ICON_SIZE / 2.0;
    const double radius = ICON_SIZE / 2.0 - 2.0;

    cairo_set_source_rgba(cr, r, g, b, 1.0);
    cairo_set_line_width(cr, 2.5);
    cairo_arc(cr, cx, cy, radius, 0, 2 * M_PI);
    if (filled) {
        cairo_fill(cr);
    } else {
        cairo_stroke(cr);
    }

    cairo_destroy(cr);
    GdkPixbuf *pixbuf =
        gdk_pixbuf_get_from_surface(surface, 0, 0, ICON_SIZE, ICON_SIZE);
    cairo_surface_destroy(surface);
    return pixbuf;
}

static void set_state(app_state_t state) {
    atomic_store(&g_app.state, state);
    GdkPixbuf *pb = NULL;
    switch (state) {
        case STATE_IDLE:       pb = g_app.icon_idle;       break;
        case STATE_RECORDING:  pb = g_app.icon_recording;  break;
        case STATE_PROCESSING: pb = g_app.icon_processing; break;
    }
    gtk_status_icon_set_from_pixbuf(g_app.status_icon, pb);
}

/* A trampoline so a background thread can ask the main thread to set the state. */
typedef struct { app_state_t state; } set_state_msg_t;

static gboolean set_state_main(gpointer data) {
    set_state_msg_t *msg = data;
    set_state(msg->state);
    g_free(msg);
    return G_SOURCE_REMOVE;
}

static void request_state(app_state_t state) {
    set_state_msg_t *msg = g_new(set_state_msg_t, 1);
    msg->state = state;
    g_idle_add(set_state_main, msg);
}

/* ------------------------------------------------------------------------- */
/*                            Clipboard / notifs                             */
/* ------------------------------------------------------------------------- */

static void copy_to_selection(const char *text, const char *selection) {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        perror("pipe");
        return;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }
    if (pid == 0) {
        /* child: stdin <- pipe, exec xclip */
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("xclip", "xclip", "-selection", selection, (char *)NULL);
        _exit(127);
    }
    /* parent: write text, close pipe, wait */
    close(pipefd[0]);
    size_t len = strlen(text);
    const char *p = text;
    while (len > 0) {
        ssize_t w = write(pipefd[1], p, len);
        if (w <= 0) break;
        p += w;
        len -= (size_t)w;
    }
    close(pipefd[1]);
    waitpid(pid, NULL, 0);
}

static void copy_to_both_clipboards(const char *text) {
    copy_to_selection(text, "primary");
    copy_to_selection(text, "clipboard");
}

typedef struct {
    char *title;
    char *body;
} notify_msg_t;

static gboolean notify_main(gpointer data) {
    notify_msg_t *msg = data;
    NotifyNotification *n =
        notify_notification_new(msg->title, msg->body, "audio-input-microphone");
    notify_notification_set_timeout(n, NOTIFY_TIMEOUT_MS);
    GError *err = NULL;
    if (!notify_notification_show(n, &err)) {
        fprintf(stderr, "notify failed: %s\n", err ? err->message : "?");
        if (err) g_error_free(err);
    }
    g_object_unref(n);
    g_free(msg->title);
    g_free(msg->body);
    g_free(msg);
    return G_SOURCE_REMOVE;
}

static void request_notification(const char *title, const char *body) {
    notify_msg_t *msg = g_new(notify_msg_t, 1);
    msg->title = g_strdup(title);
    msg->body  = g_strdup(body);
    g_idle_add(notify_main, msg);
}

/* ------------------------------------------------------------------------- */
/*                             Voice commands                                */
/* ------------------------------------------------------------------------- */

static void replace_all(char *out, const char *in, const char *needle,
                        const char *replacement) {
    const size_t nlen = strlen(needle);
    const size_t rlen = strlen(replacement);
    const char *p = in;
    char *o = out;
    while (*p) {
        if (strncmp(p, needle, nlen) == 0) {
            memcpy(o, replacement, rlen);
            o += rlen;
            p += nlen;
        } else {
            *o++ = *p++;
        }
    }
    *o = '\0';
}

static void str_lower_inplace(char *s) {
    for (; *s; ++s) {
        if ((unsigned char)*s < 128 && *s >= 'A' && *s <= 'Z') {
            *s = (char)(*s + ('a' - 'A'));
        }
    }
}

/* Apply the full voice command table. Returns a freshly malloc'd string. */
static char *apply_voice_commands(const char *text) {
    static const char *pairs[][2] = {
        {"nouvelle ligne",         "\n"},
        {"à la ligne",             "\n"},
        {"nouveau paragraphe",     "\n\n"},
        {"point virgule",          ";"},
        {"deux points",            ":"},
        {"point d'interrogation",  "?"},
        {"point d'exclamation",    "!"},
        {"ouvre parenthèse",       "("},
        {"ferme parenthèse",       ")"},
        {"ouvre guillemets",       "\""},
        {"ferme guillemets",       "\""},
        {"point",                  "."},
        {"virgule",                ","},
        {"tiret",                  "-"},
        {NULL, NULL}
    };

    size_t n = strlen(text);
    char *buf_a = malloc(n * 4 + 16);
    char *buf_b = malloc(n * 4 + 16);
    if (!buf_a || !buf_b) {
        free(buf_a); free(buf_b);
        return strdup(text);
    }

    strcpy(buf_a, text);
    str_lower_inplace(buf_a);

    char *src = buf_a;
    char *dst = buf_b;
    for (int i = 0; pairs[i][0] != NULL; ++i) {
        replace_all(dst, src, pairs[i][0], pairs[i][1]);
        char *tmp = src; src = dst; dst = tmp;
    }

    char *result = strdup(src);
    free(buf_a);
    free(buf_b);
    return result;
}

/* Capitalize the first letter and the first letter after every sentence-ending
 * punctuation mark (. ! ?). Operates in-place on ASCII a-z only; accented
 * UTF-8 characters at sentence boundaries are left as-is. */
static void capitalize_sentences(char *text) {
    if (!text || !*text) return;

    bool cap_next = true;
    for (char *p = text; *p; ++p) {
        if (cap_next && *p >= 'a' && *p <= 'z') {
            *p = (char)(*p - ('a' - 'A'));
            cap_next = false;
        } else if (*p == '.' || *p == '!' || *p == '?') {
            cap_next = true;
        } else if (*p != ' ' && *p != '\n' && *p != '\t') {
            cap_next = false;
        }
    }
}

/* ------------------------------------------------------------------------- */
/*                              PortAudio                                    */
/* ------------------------------------------------------------------------- */

static int audio_callback(const void *input, void *output,
                          unsigned long frame_count,
                          const PaStreamCallbackTimeInfo *time_info,
                          PaStreamCallbackFlags status_flags, void *user_data) {
    (void)output;
    (void)time_info;
    (void)status_flags;
    (void)user_data;

    if (!input) return paContinue;

    size_t pos = atomic_fetch_add(&g_app.audio_size, frame_count);
    if (pos + frame_count > AUDIO_CAPACITY) {
        /* Buffer full: silently drop and roll back the counter */
        atomic_fetch_sub(&g_app.audio_size, frame_count);
        return paContinue;
    }
    memcpy(g_app.audio_buffer + pos, input, frame_count * sizeof(float));
    return paContinue;
}

static int start_audio_stream(void) {
    atomic_store(&g_app.audio_size, 0);

    PaStreamParameters params = {0};
    params.device = (g_app.input_device >= 0)
                        ? g_app.input_device
                        : Pa_GetDefaultInputDevice();
    if (params.device == paNoDevice) {
        fprintf(stderr, "no input device\n");
        return -1;
    }
    params.channelCount = CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency =
        Pa_GetDeviceInfo(params.device)->defaultLowInputLatency;
    params.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(&g_app.stream, &params, NULL, SAMPLE_RATE,
                                FRAMES_PER_BUFFER, paNoFlag, audio_callback,
                                NULL);
    if (err != paNoError) {
        fprintf(stderr, "Pa_OpenStream: %s\n", Pa_GetErrorText(err));
        return -1;
    }
    err = Pa_StartStream(g_app.stream);
    if (err != paNoError) {
        fprintf(stderr, "Pa_StartStream: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(g_app.stream);
        g_app.stream = NULL;
        return -1;
    }
    return 0;
}

static void stop_audio_stream(void) {
    if (!g_app.stream) return;
    Pa_StopStream(g_app.stream);
    Pa_CloseStream(g_app.stream);
    g_app.stream = NULL;
}

/* ------------------------------------------------------------------------- */
/*                              Transcription                                */
/* ------------------------------------------------------------------------- */

static char *run_whisper(const float *samples, size_t n_samples) {
    struct whisper_full_params params =
        whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.print_realtime   = false;
    params.print_progress   = false;
    params.print_timestamps = false;
    params.print_special    = false;
    params.translate        = false;
    params.single_segment   = false;
    params.no_context       = true;
    params.suppress_blank   = true;
    params.n_threads        = 8;
    params.language         = (strcmp(g_app.language, "auto") == 0)
                                  ? NULL
                                  : g_app.language;

    if (whisper_full(g_app.whisper_ctx, params, samples, (int)n_samples) != 0) {
        return strdup("");
    }

    int n_seg = whisper_full_n_segments(g_app.whisper_ctx);
    size_t cap = 256;
    char *out = malloc(cap);
    if (!out) return NULL;
    out[0] = '\0';
    size_t used = 0;

    for (int i = 0; i < n_seg; ++i) {
        const char *text = whisper_full_get_segment_text(g_app.whisper_ctx, i);
        while (*text == ' ') text++;
        size_t tlen = strlen(text);
        if (used + tlen + 2 >= cap) {
            cap = (used + tlen + 2) * 2;
            char *nout = realloc(out, cap);
            if (!nout) { free(out); return NULL; }
            out = nout;
        }
        if (used > 0) out[used++] = ' ';
        memcpy(out + used, text, tlen);
        used += tlen;
        out[used] = '\0';
    }
    return out;
}

static void *transcribe_thread(void *arg) {
    (void)arg;

    size_t n = atomic_load(&g_app.audio_size);
    if (n < SAMPLE_RATE / 4) {  /* < 250 ms captured */
        request_notification("VoiceIn", "Empty recording");
        request_state(STATE_IDLE);
        return NULL;
    }

    double audio_duration = (double)n / SAMPLE_RATE;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    char *raw = run_whisper(g_app.audio_buffer, n);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec)
                   + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    fprintf(stderr, "[perf] audio: %.1f s | transcription: %.2f s | ratio: %.1fx realtime\n",
            audio_duration, elapsed, audio_duration / elapsed);

    if (!raw || !*raw) {
        free(raw);
        request_notification("VoiceIn", "No speech detected");
        request_state(STATE_IDLE);
        return NULL;
    }

    char *processed = apply_voice_commands(raw);
    free(raw);
    capitalize_sentences(processed);

    if (processed && *processed) {
        copy_to_both_clipboards(processed);
        request_notification("VoiceIn", processed);
    } else {
        request_notification("VoiceIn", "No speech detected");
    }

    free(processed);
    request_state(STATE_IDLE);
    return NULL;
}

/* ------------------------------------------------------------------------- */
/*                            GTK tray handlers                              */
/* ------------------------------------------------------------------------- */

static void start_recording(void) {
    if (start_audio_stream() != 0) {
        request_notification("VoiceIn", "Mic error: cannot open input stream");
        return;
    }
    set_state(STATE_RECORDING);
}

static void stop_recording(void) {
    stop_audio_stream();
    set_state(STATE_PROCESSING);

    pthread_t tid;
    if (pthread_create(&tid, NULL, transcribe_thread, NULL) != 0) {
        perror("pthread_create");
        request_state(STATE_IDLE);
        return;
    }
    pthread_detach(tid);
}

static void on_status_icon_activate(GtkStatusIcon *icon, gpointer data) {
    (void)icon;
    (void)data;
    app_state_t s = atomic_load(&g_app.state);
    if (s == STATE_IDLE) {
        start_recording();
    } else if (s == STATE_RECORDING) {
        stop_recording();
    }
    /* if PROCESSING: ignore, user is impatient */
}

static void on_menu_toggle(GtkMenuItem *item, gpointer data) {
    (void)item;
    (void)data;
    on_status_icon_activate(NULL, NULL);
}

static void on_menu_quit(GtkMenuItem *item, gpointer data) {
    (void)item;
    (void)data;
    gtk_main_quit();
}

static void on_status_icon_popup(GtkStatusIcon *icon, guint button,
                                 guint activate_time, gpointer data) {
    (void)data;
    gtk_menu_popup_at_pointer(GTK_MENU(g_app.menu), NULL);
    (void)button;
    (void)activate_time;
}

/* ------------------------------------------------------------------------- */
/*                                  Main                                     */
/* ------------------------------------------------------------------------- */

static int init_whisper(void) {
    const char *model = getenv("VOICE_IN_MODEL");
    if (!model || !*model) model = DEFAULT_MODEL_PATH;

    fprintf(stderr, "loading whisper model: %s\n", model);
    struct whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = true;
    g_app.whisper_ctx = whisper_init_from_file_with_params(model, cparams);
    if (!g_app.whisper_ctx) {
        fprintf(stderr, "failed to load model: %s\n", model);
        return -1;
    }
    fprintf(stderr, "whisper ready\n");
    return 0;
}

static void init_language(void) {
    const char *lang = getenv("VOICE_IN_LANGUAGE");
    if (!lang || !*lang) lang = DEFAULT_LANGUAGE;
    strncpy(g_app.language, lang, sizeof(g_app.language) - 1);
    g_app.language[sizeof(g_app.language) - 1] = '\0';
}

static void init_device(void) {
    const char *dev = getenv("VOICE_IN_DEVICE");
    g_app.input_device = (dev && *dev) ? atoi(dev) : -1;
}

static void build_menu(void) {
    g_app.menu = gtk_menu_new();
    GtkWidget *toggle = gtk_menu_item_new_with_label("Toggle recording");
    GtkWidget *quit   = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(toggle, "activate", G_CALLBACK(on_menu_toggle), NULL);
    g_signal_connect(quit,   "activate", G_CALLBACK(on_menu_quit),   NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(g_app.menu), toggle);
    gtk_menu_shell_append(GTK_MENU_SHELL(g_app.menu), quit);
    gtk_widget_show_all(g_app.menu);
}

static void build_icons_and_tray(void) {
    g_app.icon_idle       = make_icon_pixbuf(1.0, 1.0, 1.0, FALSE);
    g_app.icon_recording  = make_icon_pixbuf(0.94, 0.33, 0.31, TRUE);
    g_app.icon_processing = make_icon_pixbuf(1.0, 0.65, 0.15, TRUE);

    g_app.status_icon = gtk_status_icon_new_from_pixbuf(g_app.icon_idle);
    gtk_status_icon_set_tooltip_text(g_app.status_icon, "VoiceIn local");
    gtk_status_icon_set_visible(g_app.status_icon, TRUE);

    g_signal_connect(g_app.status_icon, "activate",
                     G_CALLBACK(on_status_icon_activate), NULL);
    g_signal_connect(g_app.status_icon, "popup-menu",
                     G_CALLBACK(on_status_icon_popup), NULL);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    if (!notify_init("VoiceIn")) {
        fprintf(stderr, "notify_init failed\n");
        return 1;
    }

    init_language();
    init_device();

    g_app.audio_buffer = calloc(AUDIO_CAPACITY, sizeof(float));
    if (!g_app.audio_buffer) {
        fprintf(stderr, "cannot allocate audio buffer\n");
        return 1;
    }
    atomic_store(&g_app.audio_size, 0);
    atomic_store(&g_app.state, STATE_IDLE);

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "Pa_Initialize: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    if (init_whisper() != 0) {
        Pa_Terminate();
        return 1;
    }

    build_menu();
    build_icons_and_tray();

    gtk_main();

    /* Shutdown */
    if (g_app.stream) stop_audio_stream();
    Pa_Terminate();
    whisper_free(g_app.whisper_ctx);
    notify_uninit();
    free(g_app.audio_buffer);
    g_object_unref(g_app.icon_idle);
    g_object_unref(g_app.icon_recording);
    g_object_unref(g_app.icon_processing);
    return 0;
}
