// SPDX-License-Identifier: MIT
/*
 * voice_in.c - System-tray push-to-talk dictation for Linux (X11).
 *
 * Copyright (C) 2026 Olivier Pons
 *
 * Left-click the tray icon to toggle recording. Audio is captured via
 * PortAudio, transcribed locally with whisper.cpp, pushed to both X11
 * selections (PRIMARY + CLIPBOARD) via xclip, and shown as a desktop
 * notification through libnotify.
 *
 * Environment variables:
 *   VOICE_IN_MODEL       path to ggml model
 *   VOICE_IN_LANGUAGE    ISO code or "auto" (default: "fr")
 *   VOICE_IN_DEVICE      PortAudio device index
 *   VOICE_IN_COMMANDS        "1" to enable voice commands
 *   VOICE_IN_CMDS_FILE       explicit path to commands file
 *   VOICE_IN_NOTIFY_PERSIST  "1" to keep notifications in history
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

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "whisper.h"

/* ---- defines ---- */

#ifndef M_PI
#define M_PI                3.14159265358979323846
#endif

#define SAMPLE_RATE         16000
#define NUM_CHANNELS        1
#define FRAMES_PER_BUFFER   1024
#define MAX_RECORD_SEC      600
#define AUDIO_CAPACITY      ((size_t)SAMPLE_RATE * MAX_RECORD_SEC)
#define ICON_PX             22
#define NOTIFY_TIMEOUT_MS   10000
#define WHISPER_THREADS     8
#define MIN_AUDIO_SAMPLES   (SAMPLE_RATE / 4)
#define CMD_LINE_MAX        1024
#define CMD_INITIAL_CAP     32

#define DEFAULT_MODEL       "whisper.cpp/models/ggml-medium.bin"
#define DEFAULT_LANG        "fr"
#define CMDS_DIR            "commands"

/* ---- types ---- */

enum app_state {
    STATE_IDLE,
    STATE_RECORDING,
    STATE_PROCESSING,
};

struct state_msg {
    enum app_state state;
};

struct notify_msg {
    char *title;
    char *body;
};

struct voice_cmd {
    char *spoken;
    char *replacement;
};

struct app {
    GtkStatusIcon           *status_icon;
    GdkPixbuf               *icon_idle;
    GdkPixbuf               *icon_recording;
    GdkPixbuf               *icon_processing;
    GtkWidget               *menu;
    PaStream                *stream;
    int                     input_device;
    float                   *audio_buf;
    atomic_size_t           audio_len;
    struct whisper_context   *wctx;
    char                    lang[16];
    bool                    commands_on;
    bool                    notify_persist;
    struct voice_cmd        *commands;
    int                     cmd_count;
    int                     cmd_cap;
    _Atomic enum app_state  state;
};

/* ---- static prototypes ---- */

static GdkPixbuf *make_icon(double r, double g, double b,
                            gboolean filled);
static void set_state(enum app_state state);
static gboolean set_state_cb(gpointer data);
static void request_state(enum app_state state);
static void copy_to_selection(const char *text,
                              const char *sel);
static void copy_to_clipboards(const char *text);
static gboolean notify_cb(gpointer data);
static void request_notify(const char *title,
                           const char *body);
static void process_escapes(char *s);
static int cmd_cmp_len_desc(const void *a, const void *b);
static void load_commands(void);
static void free_commands(void);
static void str_replace(char *out, const char *in,
                        const char *needle,
                        const char *repl);
static void str_lower_ascii(char *s);
static char *apply_voice_cmds(const char *text);
static void capitalize_sentences(char *text);
static int audio_cb(const void *in, void *out,
                    unsigned long count,
                    const PaStreamCallbackTimeInfo *ti,
                    PaStreamCallbackFlags flags,
                    void *udata);
static int audio_start(void);
static void audio_stop(void);
static char *run_whisper(const float *samples,
                         size_t n_samples);
static void *transcribe_thread(void *arg);
static void on_activate(GtkStatusIcon *icon, gpointer data);
static void on_toggle(GtkMenuItem *item, gpointer data);
static void on_quit(GtkMenuItem *item, gpointer data);
static void on_popup(GtkStatusIcon *icon, guint btn,
                     guint time, gpointer data);
static void rec_start(void);
static void rec_stop(void);
static int init_whisper(void);
static void init_lang(void);
static void init_device(void);
static void init_options(void);
static void build_menu(void);
static void build_tray(void);

/* ---- global state ---- */

static struct app g_app;

/* ---- icons ---- */

/**
 * make_icon - Render a colored circle as a GdkPixbuf.
 * @r: red component (0.0 to 1.0).
 * @g: green component (0.0 to 1.0).
 * @b: blue component (0.0 to 1.0).
 * @filled: TRUE for solid fill, FALSE for outline only.
 *
 * Return: newly allocated GdkPixbuf (caller owns the ref).
 */
static GdkPixbuf *make_icon(double r, double g, double b,
                            gboolean filled)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    GdkPixbuf *pb;
    double cx = ICON_PX / 2.0;
    double radius = cx - 2.0;

    surface = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, ICON_PX, ICON_PX);
    cr = cairo_create(surface);
    cairo_set_source_rgba(cr, r, g, b, 1.0);
    cairo_set_line_width(cr, 2.5);
    cairo_arc(cr, cx, cx, radius, 0, 2 * M_PI);
    if (filled)
        cairo_fill(cr);
    else
        cairo_stroke(cr);
    cairo_destroy(cr);
    pb = gdk_pixbuf_get_from_surface(
        surface, 0, 0, ICON_PX, ICON_PX);
    cairo_surface_destroy(surface);
    return pb;
}

/* ---- state management ---- */

static void set_state(enum app_state state)
{
    GdkPixbuf *pb;

    atomic_store(&g_app.state, state);
    pb = g_app.icon_idle;
    if (state == STATE_RECORDING)
        pb = g_app.icon_recording;
    else if (state == STATE_PROCESSING)
        pb = g_app.icon_processing;
    gtk_status_icon_set_from_pixbuf(g_app.status_icon, pb);
}

static gboolean set_state_cb(gpointer data)
{
    struct state_msg *msg = data;

    set_state(msg->state);
    g_free(msg);
    return G_SOURCE_REMOVE;
}

static void request_state(enum app_state state)
{
    struct state_msg *msg;

    msg = g_new(struct state_msg, 1);
    msg->state = state;
    g_idle_add(set_state_cb, msg);
}

/* ---- clipboard / notifications ---- */

/**
 * copy_to_selection - Pipe text into xclip for one X11 selection.
 * @text: NUL-terminated string to copy.
 * @sel: "primary" or "clipboard".
 */
static void copy_to_selection(const char *text, const char *sel)
{
    int pipefd[2];
    pid_t pid;
    size_t len;
    const char *p;
    ssize_t w;

    if (pipe(pipefd) != 0)
        return;
    pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }
    if (pid == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("xclip", "xclip", "-selection",
               sel, (char *)NULL);
        _exit(127);
    }
    close(pipefd[0]);
    len = strlen(text);
    p = text;
    while (len > 0) {
        w = write(pipefd[1], p, len);
        if (w <= 0)
            break;
        p += w;
        len -= (size_t)w;
    }
    close(pipefd[1]);
    waitpid(pid, NULL, 0);
}

static void copy_to_clipboards(const char *text)
{
    copy_to_selection(text, "primary");
    copy_to_selection(text, "clipboard");
}

static gboolean notify_cb(gpointer data)
{
    struct notify_msg *msg = data;
    NotifyNotification *n;
    GError *err = NULL;

    n = notify_notification_new(
        msg->title, msg->body, "audio-input-microphone");
    notify_notification_set_timeout(n, NOTIFY_TIMEOUT_MS);
    if (!g_app.notify_persist)
        notify_notification_set_hint(
            n, "transient",
            g_variant_new_boolean(TRUE));
    if (!notify_notification_show(n, &err)) {
        fprintf(stderr, "notify: %s\n",
                err ? err->message : "unknown error");
        if (err)
            g_error_free(err);
    }
    g_object_unref(n);
    g_free(msg->title);
    g_free(msg->body);
    g_free(msg);
    return G_SOURCE_REMOVE;
}

static void request_notify(const char *title, const char *body)
{
    struct notify_msg *msg;

    msg = g_new(struct notify_msg, 1);
    msg->title = g_strdup(title);
    msg->body = g_strdup(body);
    g_idle_add(notify_cb, msg);
}

/* ---- voice commands (file-based) ---- */

/**
 * process_escapes - Expand \n and \t escape sequences in place.
 * @s: NUL-terminated string to modify.
 */
static void process_escapes(char *s)
{
    char *r = s;
    char *w = s;

    while (*r) {
        if (*r == '\\' && *(r + 1) == 'n') {
            *w++ = '\n';
            r += 2;
        } else if (*r == '\\' && *(r + 1) == 't') {
            *w++ = '\t';
            r += 2;
        } else {
            *w++ = *r++;
        }
    }
    *w = '\0';
}

/*
 * Sort comparator: longest spoken form first, so "open parenthesis"
 * is tried before "open" and partial matches are avoided.
 */
static int cmd_cmp_len_desc(const void *a, const void *b)
{
    const struct voice_cmd *ca = a;
    const struct voice_cmd *cb = b;
    size_t la = strlen(ca->spoken);
    size_t lb = strlen(cb->spoken);

    if (la > lb)
        return -1;
    if (la < lb)
        return 1;
    return 0;
}

/**
 * load_commands - Read voice command pairs from a text file.
 *
 * Looks for VOICE_IN_CMDS_FILE first, then commands/<lang>.txt
 * where <lang> is the first two characters of VOICE_IN_LANGUAGE.
 * Does nothing if commands are disabled or no file is found.
 */
static void load_commands(void)
{
    const char *path;
    char auto_path[256];
    FILE *fp;
    char line[CMD_LINE_MAX];
    char *sep;
    size_t slen;

    if (!g_app.commands_on)
        return;
    path = getenv("VOICE_IN_CMDS_FILE");
    if (!path || !*path) {
        snprintf(auto_path, sizeof(auto_path),
                 "%s/%.2s.txt", CMDS_DIR, g_app.lang);
        path = auto_path;
    }
    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "voice commands: %s not found\n",
                path);
        return;
    }
    g_app.cmd_cap = CMD_INITIAL_CAP;
    g_app.commands = malloc(
        g_app.cmd_cap * sizeof(*g_app.commands));
    if (!g_app.commands) {
        fclose(fp);
        return;
    }
    g_app.cmd_count = 0;
    while (fgets(line, CMD_LINE_MAX, fp)) {
        slen = strlen(line);
        while (slen > 0 &&
               (line[slen - 1] == '\n' ||
                line[slen - 1] == '\r'))
            line[--slen] = '\0';
        if (slen == 0 || line[0] == '#')
            continue;
        sep = strchr(line, '|');
        if (!sep)
            continue;
        *sep = '\0';
        process_escapes(sep + 1);
        if (g_app.cmd_count >= g_app.cmd_cap) {
            g_app.cmd_cap *= 2;
            g_app.commands = realloc(g_app.commands,
                g_app.cmd_cap * sizeof(*g_app.commands));
            if (!g_app.commands)
                break;
        }
        g_app.commands[g_app.cmd_count].spoken =
            strdup(line);
        g_app.commands[g_app.cmd_count].replacement =
            strdup(sep + 1);
        g_app.cmd_count++;
    }
    fclose(fp);
    qsort(g_app.commands, g_app.cmd_count,
          sizeof(*g_app.commands), cmd_cmp_len_desc);
    fprintf(stderr, "voice commands: loaded %d from %s\n",
            g_app.cmd_count, path);
}

static void free_commands(void)
{
    int i;

    for (i = 0; i < g_app.cmd_count; i++) {
        free(g_app.commands[i].spoken);
        free(g_app.commands[i].replacement);
    }
    free(g_app.commands);
    g_app.commands = NULL;
    g_app.cmd_count = 0;
}

/* ---- text processing ---- */

static void str_replace(char *out, const char *in,
                        const char *needle, const char *repl)
{
    size_t nlen = strlen(needle);
    size_t rlen = strlen(repl);
    const char *p = in;
    char *o = out;

    while (*p) {
        if (strncmp(p, needle, nlen) == 0) {
            memcpy(o, repl, rlen);
            o += rlen;
            p += nlen;
        } else {
            *o++ = *p++;
        }
    }
    *o = '\0';
}

static void str_lower_ascii(char *s)
{
    for (; *s; ++s) {
        if ((unsigned char)*s < 128 && *s >= 'A' && *s <= 'Z')
            *s = (char)(*s + ('a' - 'A'));
    }
}

/**
 * apply_voice_cmds - Replace spoken commands from the loaded table.
 * @text: raw transcript from whisper.
 *
 * Return: newly allocated string (caller must free).
 */
static char *apply_voice_cmds(const char *text)
{
    size_t n = strlen(text);
    char *buf_a;
    char *buf_b;
    char *src;
    char *dst;
    char *tmp;
    char *result;
    int i;

    buf_a = malloc(n * 4 + 16);
    buf_b = malloc(n * 4 + 16);
    if (!buf_a || !buf_b) {
        free(buf_a);
        free(buf_b);
        return strdup(text);
    }
    strcpy(buf_a, text);
    str_lower_ascii(buf_a);
    src = buf_a;
    dst = buf_b;
    for (i = 0; i < g_app.cmd_count; i++) {
        str_replace(dst, src,
                    g_app.commands[i].spoken,
                    g_app.commands[i].replacement);
        tmp = src;
        src = dst;
        dst = tmp;
    }
    result = strdup(src);
    free(buf_a);
    free(buf_b);
    return result;
}

/**
 * capitalize_sentences - Uppercase the first letter of each sentence.
 * @text: NUL-terminated string to modify in place.
 *
 * Only handles ASCII a-z; accented UTF-8 characters at sentence
 * boundaries are left as-is.
 */
static void capitalize_sentences(char *text)
{
    bool cap_next = true;
    char *p;

    if (!text || !*text)
        return;
    for (p = text; *p; ++p) {
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

/* ---- PortAudio ---- */

static int audio_cb(const void *in, void *out,
                    unsigned long count,
                    const PaStreamCallbackTimeInfo *ti,
                    PaStreamCallbackFlags flags, void *udata)
{
    size_t pos;

    (void)out;
    (void)ti;
    (void)flags;
    (void)udata;
    if (!in)
        return paContinue;
    pos = atomic_fetch_add(&g_app.audio_len, count);
    if (pos + count > AUDIO_CAPACITY) {
        atomic_fetch_sub(&g_app.audio_len, count);
        return paContinue;
    }
    memcpy(g_app.audio_buf + pos, in,
           count * sizeof(float));
    return paContinue;
}

static int audio_start(void)
{
    PaStreamParameters params;
    PaError err;

    atomic_store(&g_app.audio_len, 0);
    memset(&params, 0, sizeof(params));
    if (g_app.input_device >= 0)
        params.device = g_app.input_device;
    else
        params.device = Pa_GetDefaultInputDevice();
    if (params.device == paNoDevice) {
        fprintf(stderr, "no input device\n");
        return -1;
    }
    params.channelCount = NUM_CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency =
        Pa_GetDeviceInfo(params.device)
            ->defaultLowInputLatency;
    err = Pa_OpenStream(&g_app.stream, &params, NULL,
                        SAMPLE_RATE, FRAMES_PER_BUFFER,
                        paNoFlag, audio_cb, NULL);
    if (err != paNoError) {
        fprintf(stderr, "Pa_OpenStream: %s\n",
                Pa_GetErrorText(err));
        return -1;
    }
    err = Pa_StartStream(g_app.stream);
    if (err != paNoError) {
        fprintf(stderr, "Pa_StartStream: %s\n",
                Pa_GetErrorText(err));
        Pa_CloseStream(g_app.stream);
        g_app.stream = NULL;
        return -1;
    }
    return 0;
}

static void audio_stop(void)
{
    if (!g_app.stream)
        return;
    Pa_StopStream(g_app.stream);
    Pa_CloseStream(g_app.stream);
    g_app.stream = NULL;
}

/* ---- whisper transcription ---- */

/**
 * run_whisper - Transcribe a float32 PCM buffer.
 * @samples: mono 16 kHz float32 audio.
 * @n_samples: number of samples.
 *
 * Return: newly allocated string (caller must free).
 */
static char *run_whisper(const float *samples, size_t n_samples)
{
    struct whisper_full_params wp;
    int n_seg;
    int i;
    const char *seg_text;
    size_t cap = 256;
    size_t used = 0;
    size_t tlen;
    char *out;
    char *nout;

    wp = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wp.print_realtime = false;
    wp.print_progress = false;
    wp.print_timestamps = false;
    wp.print_special = false;
    wp.translate = false;
    wp.single_segment = false;
    wp.no_context = true;
    wp.suppress_blank = true;
    wp.n_threads = WHISPER_THREADS;
    wp.language = NULL;
    if (strcmp(g_app.lang, "auto") != 0)
        wp.language = g_app.lang;
    if (whisper_full(g_app.wctx, wp, samples,
                     (int)n_samples) != 0)
        return strdup("");
    n_seg = whisper_full_n_segments(g_app.wctx);
    out = malloc(cap);
    if (!out)
        return NULL;
    out[0] = '\0';
    for (i = 0; i < n_seg; i++) {
        seg_text = whisper_full_get_segment_text(
            g_app.wctx, i);
        while (*seg_text == ' ')
            seg_text++;
        tlen = strlen(seg_text);
        if (used + tlen + 2 >= cap) {
            cap = (used + tlen + 2) * 2;
            nout = realloc(out, cap);
            if (!nout) {
                free(out);
                return NULL;
            }
            out = nout;
        }
        if (used > 0)
            out[used++] = ' ';
        memcpy(out + used, seg_text, tlen);
        used += tlen;
        out[used] = '\0';
    }
    return out;
}

/**
 * transcribe_thread - Worker: stop audio, run whisper, publish.
 * @arg: unused.
 *
 * Runs on a detached thread spawned by rec_stop().
 */
static void *transcribe_thread(void *arg)
{
    size_t n;
    double audio_sec;
    double elapsed;
    struct timespec t0;
    struct timespec t1;
    char *raw;
    char *text;

    (void)arg;
    n = atomic_load(&g_app.audio_len);
    if (n < MIN_AUDIO_SAMPLES) {
        request_notify("VoiceIn", "Empty recording");
        request_state(STATE_IDLE);
        return NULL;
    }
    audio_sec = (double)n / SAMPLE_RATE;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    raw = run_whisper(g_app.audio_buf, n);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed = (t1.tv_sec - t0.tv_sec)
            + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    fprintf(stderr,
            "[perf] audio: %.1f s | transcription: %.2f s"
            " | ratio: %.1fx realtime\n",
            audio_sec, elapsed, audio_sec / elapsed);
    if (!raw || !*raw) {
        free(raw);
        request_notify("VoiceIn", "No speech detected");
        request_state(STATE_IDLE);
        return NULL;
    }
    if (g_app.commands_on && g_app.cmd_count > 0) {
        text = apply_voice_cmds(raw);
        free(raw);
    } else {
        text = raw;
    }
    capitalize_sentences(text);
    if (text && *text) {
        copy_to_clipboards(text);
        request_notify("VoiceIn", text);
    } else {
        request_notify("VoiceIn", "No speech detected");
    }
    free(text);
    request_state(STATE_IDLE);
    return NULL;
}

/* ---- recording control ---- */

static void rec_start(void)
{
    if (audio_start() != 0) {
        request_notify("VoiceIn",
                       "Mic error: cannot open stream");
        return;
    }
    set_state(STATE_RECORDING);
}

static void rec_stop(void)
{
    pthread_t tid;

    audio_stop();
    set_state(STATE_PROCESSING);
    if (pthread_create(&tid, NULL,
                       transcribe_thread, NULL) != 0) {
        perror("pthread_create");
        request_state(STATE_IDLE);
        return;
    }
    pthread_detach(tid);
}

/* ---- GTK tray handlers ---- */

static void on_activate(GtkStatusIcon *icon, gpointer data)
{
    enum app_state s;

    (void)icon;
    (void)data;
    s = atomic_load(&g_app.state);
    if (s == STATE_IDLE)
        rec_start();
    else if (s == STATE_RECORDING)
        rec_stop();
}

static void on_toggle(GtkMenuItem *item, gpointer data)
{
    (void)item;
    (void)data;
    on_activate(NULL, NULL);
}

static void on_quit(GtkMenuItem *item, gpointer data)
{
    (void)item;
    (void)data;
    gtk_main_quit();
}

static void on_popup(GtkStatusIcon *icon, guint btn,
                     guint time, gpointer data)
{
    (void)icon;
    (void)btn;
    (void)time;
    (void)data;
    gtk_menu_popup_at_pointer(GTK_MENU(g_app.menu), NULL);
}

/* ---- initialization ---- */

static int init_whisper(void)
{
    const char *model;
    struct whisper_context_params cp;

    model = getenv("VOICE_IN_MODEL");
    if (!model || !*model)
        model = DEFAULT_MODEL;
    fprintf(stderr, "loading whisper model: %s\n", model);
    cp = whisper_context_default_params();
    cp.use_gpu = true;
    g_app.wctx =
        whisper_init_from_file_with_params(model, cp);
    if (!g_app.wctx) {
        fprintf(stderr, "failed to load model: %s\n",
                model);
        return -1;
    }
    fprintf(stderr, "whisper ready\n");
    return 0;
}

static void init_lang(void)
{
    const char *lang;

    lang = getenv("VOICE_IN_LANGUAGE");
    if (!lang || !*lang)
        lang = DEFAULT_LANG;
    strncpy(g_app.lang, lang, sizeof(g_app.lang) - 1);
    g_app.lang[sizeof(g_app.lang) - 1] = '\0';
}

static void init_device(void)
{
    const char *dev;

    dev = getenv("VOICE_IN_DEVICE");
    g_app.input_device = (dev && *dev) ? atoi(dev) : -1;
}

static void init_options(void)
{
    const char *cmds;
    const char *persist;

    cmds = getenv("VOICE_IN_COMMANDS");
    g_app.commands_on = (cmds && strcmp(cmds, "1") == 0);
    fprintf(stderr, "voice commands: %s\n",
            g_app.commands_on ? "enabled" : "disabled");
    persist = getenv("VOICE_IN_NOTIFY_PERSIST");
    g_app.notify_persist =
        (persist && strcmp(persist, "1") == 0);
    fprintf(stderr, "notifications: %s\n",
            g_app.notify_persist
                ? "persistent" : "transient");
}

static void build_menu(void)
{
    GtkWidget *toggle;
    GtkWidget *quit;

    g_app.menu = gtk_menu_new();
    toggle = gtk_menu_item_new_with_label(
        "Toggle recording");
    quit = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(toggle, "activate",
                     G_CALLBACK(on_toggle), NULL);
    g_signal_connect(quit, "activate",
                     G_CALLBACK(on_quit), NULL);
    gtk_menu_shell_append(
        GTK_MENU_SHELL(g_app.menu), toggle);
    gtk_menu_shell_append(
        GTK_MENU_SHELL(g_app.menu), quit);
    gtk_widget_show_all(g_app.menu);
}

static void build_tray(void)
{
    g_app.icon_idle =
        make_icon(1.0, 1.0, 1.0, FALSE);
    g_app.icon_recording =
        make_icon(0.94, 0.33, 0.31, TRUE);
    g_app.icon_processing =
        make_icon(1.0, 0.65, 0.15, TRUE);
    g_app.status_icon =
        gtk_status_icon_new_from_pixbuf(g_app.icon_idle);
    gtk_status_icon_set_tooltip_text(
        g_app.status_icon, "VoiceIn local");
    gtk_status_icon_set_visible(g_app.status_icon, TRUE);
    g_signal_connect(g_app.status_icon, "activate",
                     G_CALLBACK(on_activate), NULL);
    g_signal_connect(g_app.status_icon, "popup-menu",
                     G_CALLBACK(on_popup), NULL);
}

/* ---- main ---- */

int main(int argc, char **argv)
{
    PaError err;

    gtk_init(&argc, &argv);
    if (!notify_init("VoiceIn")) {
        fprintf(stderr, "notify_init failed\n");
        return 1;
    }
    init_lang();
    init_device();
    init_options();
    load_commands();
    g_app.audio_buf = calloc(AUDIO_CAPACITY,
                             sizeof(*g_app.audio_buf));
    if (!g_app.audio_buf) {
        fprintf(stderr, "cannot allocate audio buffer\n");
        return 1;
    }
    atomic_store(&g_app.audio_len, 0);
    atomic_store(&g_app.state, STATE_IDLE);
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "Pa_Initialize: %s\n",
                Pa_GetErrorText(err));
        return 1;
    }
    if (init_whisper() != 0) {
        Pa_Terminate();
        return 1;
    }
    build_menu();
    build_tray();
    gtk_main();

    /* shutdown */
    if (g_app.stream)
        audio_stop();
    Pa_Terminate();
    whisper_free(g_app.wctx);
    free_commands();
    notify_uninit();
    free(g_app.audio_buf);
    g_app.audio_buf = NULL;
    g_object_unref(g_app.icon_idle);
    g_object_unref(g_app.icon_recording);
    g_object_unref(g_app.icon_processing);
    return 0;
}
