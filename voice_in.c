/*
** file   : voice_in.c
** author : Olivier Pons
** date   : 2026-04-16
**
** System-tray push-to-talk dictation for Linux (X11).
** Left-click the tray icon to toggle recording. Audio is captured via
** PortAudio, transcribed locally with whisper.cpp, pushed to both X11
** selections (PRIMARY + CLIPBOARD) via xclip, and shown as a desktop
** notification through libnotify.
**
** Environment variables:
**   VOICE_IN_MODEL     path to ggml model (default: see DEFAULT_MODEL)
**   VOICE_IN_LANGUAGE  ISO code or "auto" (default: "fr")
**   VOICE_IN_DEVICE    PortAudio device index (default: system default)
**   VOICE_IN_COMMANDS  set to "1" to enable voice commands (default: off)
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

/* -------------------------------------------------- */
/*                  Defines                           */
/* -------------------------------------------------- */

#ifndef M_PI
# define M_PI 3.14159265358979323846
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

#define DEFAULT_MODEL       "whisper.cpp/models/ggml-medium.bin"
#define DEFAULT_LANG        "fr"

/* -------------------------------------------------- */
/*                  Types                             */
/* -------------------------------------------------- */

typedef enum e_app_state
{
	STATE_IDLE,
	STATE_RECORDING,
	STATE_PROCESSING
}	t_app_state;

typedef struct s_state_msg
{
	t_app_state	state;
}	t_state_msg;

typedef struct s_notify_msg
{
	char	*title;
	char	*body;
}	t_notify_msg;

typedef struct s_app
{
	GtkStatusIcon		*status_icon;
	GdkPixbuf		*icon_idle;
	GdkPixbuf		*icon_recording;
	GdkPixbuf		*icon_processing;
	GtkWidget		*menu;
	PaStream		*stream;
	int			input_device;
	float			*audio_buf;
	atomic_size_t		audio_len;
	struct whisper_context	*wctx;
	char			lang[16];
	bool			commands_on;
	_Atomic t_app_state	state;
}	t_app;

/*
** Voice command substitution pairs.
** Spoken form (lowercase) → replacement string.
** Longer phrases must come first to avoid partial matches.
*/
static const char	*g_voice_pairs[][2] = {
	{"nouvelle ligne",        "\n"},
	{"a la ligne",            "\n"},
	{"nouveau paragraphe",    "\n\n"},
	{"point virgule",         ";"},
	{"deux points",           ":"},
	{"point d'interrogation", "?"},
	{"point d'exclamation",   "!"},
	{"ouvre parenthese",      "("},
	{"ferme parenthese",      ")"},
	{"ouvre guillemets",       "\""},
	{"ferme guillemets",       "\""},
	{"point",                 "."},
	{"virgule",               ","},
	{"tiret",                 "-"},
	{NULL, NULL}
};

/* -------------------------------------------------- */
/*              Static prototypes                     */
/* -------------------------------------------------- */

/* Icons */
static GdkPixbuf	*make_icon(double r, double g, double b,
				gboolean filled);

/* State */
static void		set_state(t_app_state state);
static gboolean		set_state_cb(gpointer data);
static void		request_state(t_app_state state);

/* Clipboard and notifications */
static void		copy_to_selection(const char *text,
				const char *sel);
static void		copy_to_clipboards(const char *text);
static gboolean		notify_cb(gpointer data);
static void		request_notify(const char *title,
				const char *body);

/* Text processing */
static void		str_replace(char *out, const char *in,
				const char *needle, const char *repl);
static void		str_lower_ascii(char *s);
static char		*apply_voice_cmds(const char *text);
static void		capitalize_sentences(char *text);

/* Audio */
static int		audio_cb(const void *in, void *out,
				unsigned long count,
				const PaStreamCallbackTimeInfo *ti,
				PaStreamCallbackFlags flags,
				void *udata);
static int		audio_start(void);
static void		audio_stop(void);

/* Whisper */
static char		*run_whisper(const float *samples,
				size_t n_samples);
static void		*transcribe_thread(void *arg);

/* GTK handlers */
static void		on_activate(GtkStatusIcon *icon, gpointer data);
static void		on_toggle(GtkMenuItem *item, gpointer data);
static void		on_quit(GtkMenuItem *item, gpointer data);
static void		on_popup(GtkStatusIcon *icon, guint btn,
				guint time, gpointer data);

/* Recording control */
static void		rec_start(void);
static void		rec_stop(void);

/* Init helpers */
static int		init_whisper(void);
static void		init_lang(void);
static void		init_device(void);
static void		init_options(void);
static void		build_menu(void);
static void		build_tray(void);

/* -------------------------------------------------- */
/*                  Global state                      */
/* -------------------------------------------------- */

static t_app	g_app;

/* -------------------------------------------------- */
/*                  Icons                             */
/* -------------------------------------------------- */

/*
** make_icon - Render a colored circle as a GdkPixbuf.
**
** @r, @g, @b : RGB color components (0.0 to 1.0).
** @filled    : TRUE for solid fill, FALSE for outline only.
**
** Returns a newly allocated GdkPixbuf (caller owns the ref).
*/
static GdkPixbuf	*make_icon(double r, double g, double b,
				gboolean filled)
{
	cairo_surface_t	*surface;
	cairo_t		*cr;
	GdkPixbuf	*pb;
	double		cx;
	double		radius;

	cx = ICON_PX / 2.0;
	radius = cx - 2.0;
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
	return (pb);
}

/* -------------------------------------------------- */
/*                  State management                  */
/* -------------------------------------------------- */

static void	set_state(t_app_state state)
{
	GdkPixbuf	*pb;

	atomic_store(&g_app.state, state);
	pb = g_app.icon_idle;
	if (state == STATE_RECORDING)
		pb = g_app.icon_recording;
	else if (state == STATE_PROCESSING)
		pb = g_app.icon_processing;
	gtk_status_icon_set_from_pixbuf(g_app.status_icon, pb);
}

/*
** set_state_cb - GLib idle callback to set state on the main thread.
*/
static gboolean	set_state_cb(gpointer data)
{
	t_state_msg	*msg;

	msg = data;
	set_state(msg->state);
	g_free(msg);
	return (G_SOURCE_REMOVE);
}

/*
** request_state - Thread-safe state change via g_idle_add.
*/
static void	request_state(t_app_state state)
{
	t_state_msg	*msg;

	msg = g_new(t_state_msg, 1);
	msg->state = state;
	g_idle_add(set_state_cb, msg);
}

/* -------------------------------------------------- */
/*              Clipboard / notifications             */
/* -------------------------------------------------- */

/*
** copy_to_selection - Pipe text into xclip for a given X11 selection.
**
** @text : null-terminated string to copy.
** @sel  : "primary" or "clipboard".
*/
static void	copy_to_selection(const char *text, const char *sel)
{
	int		pipefd[2];
	pid_t		pid;
	size_t		len;
	const char	*p;
	ssize_t		w;

	if (pipe(pipefd) != 0)
		return ;
	pid = fork();
	if (pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return ;
	}
	if (pid == 0)
	{
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		execlp("xclip", "xclip", "-selection", sel, (char *)NULL);
		_exit(127);
	}
	close(pipefd[0]);
	len = strlen(text);
	p = text;
	while (len > 0)
	{
		w = write(pipefd[1], p, len);
		if (w <= 0)
			break ;
		p += w;
		len -= (size_t)w;
	}
	close(pipefd[1]);
	waitpid(pid, NULL, 0);
}

static void	copy_to_clipboards(const char *text)
{
	copy_to_selection(text, "primary");
	copy_to_selection(text, "clipboard");
}

static gboolean	notify_cb(gpointer data)
{
	t_notify_msg		*msg;
	NotifyNotification	*n;
	GError			*err;

	msg = data;
	n = notify_notification_new(
		msg->title, msg->body, "audio-input-microphone");
	notify_notification_set_timeout(n, NOTIFY_TIMEOUT_MS);
	err = NULL;
	if (!notify_notification_show(n, &err))
	{
		fprintf(stderr, "notify: %s\n",
			err ? err->message : "unknown error");
		if (err)
			g_error_free(err);
	}
	g_object_unref(n);
	g_free(msg->title);
	g_free(msg->body);
	g_free(msg);
	return (G_SOURCE_REMOVE);
}

static void	request_notify(const char *title, const char *body)
{
	t_notify_msg	*msg;

	msg = g_new(t_notify_msg, 1);
	msg->title = g_strdup(title);
	msg->body = g_strdup(body);
	g_idle_add(notify_cb, msg);
}

/* -------------------------------------------------- */
/*              Text processing                       */
/* -------------------------------------------------- */

/*
** str_replace - Replace all occurrences of needle in @in,
**               writing the result into @out.
**
** @out must be large enough to hold the expanded result.
*/
static void	str_replace(char *out, const char *in,
			const char *needle, const char *repl)
{
	size_t		nlen;
	size_t		rlen;
	const char	*p;
	char		*o;

	nlen = strlen(needle);
	rlen = strlen(repl);
	p = in;
	o = out;
	while (*p)
	{
		if (strncmp(p, needle, nlen) == 0)
		{
			memcpy(o, repl, rlen);
			o += rlen;
			p += nlen;
		}
		else
			*o++ = *p++;
	}
	*o = '\0';
}

static void	str_lower_ascii(char *s)
{
	for (; *s; ++s)
	{
		if ((unsigned char)*s < 128 && *s >= 'A' && *s <= 'Z')
			*s = (char)(*s + ('a' - 'A'));
	}
}

/*
** apply_voice_cmds - Replace spoken commands with their text equivalent.
**
** @text : raw transcript from whisper.
**
** Returns a newly allocated string (caller must free).
*/
static char	*apply_voice_cmds(const char *text)
{
	size_t	n;
	char	*buf_a;
	char	*buf_b;
	char	*src;
	char	*dst;
	char	*tmp;
	char	*result;
	int	i;

	n = strlen(text);
	buf_a = malloc(n * 4 + 16);
	buf_b = malloc(n * 4 + 16);
	if (!buf_a || !buf_b)
	{
		free(buf_a);
		free(buf_b);
		return (strdup(text));
	}
	strcpy(buf_a, text);
	str_lower_ascii(buf_a);
	src = buf_a;
	dst = buf_b;
	i = 0;
	while (g_voice_pairs[i][0] != NULL)
	{
		str_replace(dst, src, g_voice_pairs[i][0],
			g_voice_pairs[i][1]);
		tmp = src;
		src = dst;
		dst = tmp;
		i++;
	}
	result = strdup(src);
	free(buf_a);
	free(buf_b);
	return (result);
}

/*
** capitalize_sentences - Uppercase the first letter of each sentence.
**
** Operates in-place. Only handles ASCII a-z; accented UTF-8 characters
** at sentence boundaries are left as-is.
*/
static void	capitalize_sentences(char *text)
{
	bool	cap_next;
	char	*p;

	if (!text || !*text)
		return ;
	cap_next = true;
	p = text;
	while (*p)
	{
		if (cap_next && *p >= 'a' && *p <= 'z')
		{
			*p = (char)(*p - ('a' - 'A'));
			cap_next = false;
		}
		else if (*p == '.' || *p == '!' || *p == '?')
			cap_next = true;
		else if (*p != ' ' && *p != '\n' && *p != '\t')
			cap_next = false;
		++p;
	}
}

/* -------------------------------------------------- */
/*                  PortAudio                         */
/* -------------------------------------------------- */

static int	audio_cb(const void *in, void *out,
			unsigned long count,
			const PaStreamCallbackTimeInfo *ti,
			PaStreamCallbackFlags flags, void *udata)
{
	size_t	pos;

	(void)out;
	(void)ti;
	(void)flags;
	(void)udata;
	if (!in)
		return (paContinue);
	pos = atomic_fetch_add(&g_app.audio_len, count);
	if (pos + count > AUDIO_CAPACITY)
	{
		atomic_fetch_sub(&g_app.audio_len, count);
		return (paContinue);
	}
	memcpy(g_app.audio_buf + pos, in, count * sizeof(float));
	return (paContinue);
}

static int	audio_start(void)
{
	PaStreamParameters	params;
	PaError			err;

	atomic_store(&g_app.audio_len, 0);
	memset(&params, 0, sizeof(params));
	if (g_app.input_device >= 0)
		params.device = g_app.input_device;
	else
		params.device = Pa_GetDefaultInputDevice();
	if (params.device == paNoDevice)
	{
		fprintf(stderr, "no input device\n");
		return (-1);
	}
	params.channelCount = NUM_CHANNELS;
	params.sampleFormat = paFloat32;
	params.suggestedLatency =
		Pa_GetDeviceInfo(params.device)->defaultLowInputLatency;
	err = Pa_OpenStream(&g_app.stream, &params, NULL,
		SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag,
		audio_cb, NULL);
	if (err != paNoError)
	{
		fprintf(stderr, "Pa_OpenStream: %s\n",
			Pa_GetErrorText(err));
		return (-1);
	}
	err = Pa_StartStream(g_app.stream);
	if (err != paNoError)
	{
		fprintf(stderr, "Pa_StartStream: %s\n",
			Pa_GetErrorText(err));
		Pa_CloseStream(g_app.stream);
		g_app.stream = NULL;
		return (-1);
	}
	return (0);
}

static void	audio_stop(void)
{
	if (!g_app.stream)
		return ;
	Pa_StopStream(g_app.stream);
	Pa_CloseStream(g_app.stream);
	g_app.stream = NULL;
}

/* -------------------------------------------------- */
/*              Whisper transcription                  */
/* -------------------------------------------------- */

/*
** run_whisper - Transcribe a float32 PCM buffer.
**
** @samples   : mono 16 kHz float32 audio.
** @n_samples : number of samples.
**
** Returns a newly allocated string (caller must free).
*/
static char	*run_whisper(const float *samples, size_t n_samples)
{
	struct whisper_full_params	wp;
	int				n_seg;
	int				i;
	const char			*seg_text;
	size_t				cap;
	size_t				used;
	size_t				tlen;
	char				*out;
	char				*nout;

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
	if (whisper_full(g_app.wctx, wp, samples, (int)n_samples) != 0)
		return (strdup(""));
	n_seg = whisper_full_n_segments(g_app.wctx);
	cap = 256;
	out = malloc(cap);
	if (!out)
		return (NULL);
	out[0] = '\0';
	used = 0;
	i = -1;
	while (++i < n_seg)
	{
		seg_text = whisper_full_get_segment_text(g_app.wctx, i);
		while (*seg_text == ' ')
			seg_text++;
		tlen = strlen(seg_text);
		if (used + tlen + 2 >= cap)
		{
			cap = (used + tlen + 2) * 2;
			nout = realloc(out, cap);
			if (!nout)
			{
				free(out);
				return (NULL);
			}
			out = nout;
		}
		if (used > 0)
			out[used++] = ' ';
		memcpy(out + used, seg_text, tlen);
		used += tlen;
		out[used] = '\0';
	}
	return (out);
}

/*
** transcribe_thread - Worker: stop audio, run whisper, publish result.
**
** Runs on a detached thread spawned by rec_stop().
*/
static void	*transcribe_thread(void *arg)
{
	size_t		n;
	double		audio_sec;
	double		elapsed;
	struct timespec	t0;
	struct timespec	t1;
	char		*raw;
	char		*text;

	(void)arg;
	n = atomic_load(&g_app.audio_len);
	if (n < MIN_AUDIO_SAMPLES)
	{
		request_notify("VoiceIn", "Empty recording");
		request_state(STATE_IDLE);
		return (NULL);
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
	if (!raw || !*raw)
	{
		free(raw);
		request_notify("VoiceIn", "No speech detected");
		request_state(STATE_IDLE);
		return (NULL);
	}
	if (g_app.commands_on)
	{
		text = apply_voice_cmds(raw);
		free(raw);
	}
	else
		text = raw;
	capitalize_sentences(text);
	if (text && *text)
	{
		copy_to_clipboards(text);
		request_notify("VoiceIn", text);
	}
	else
		request_notify("VoiceIn", "No speech detected");
	free(text);
	request_state(STATE_IDLE);
	return (NULL);
}

/* -------------------------------------------------- */
/*              Recording control                     */
/* -------------------------------------------------- */

static void	rec_start(void)
{
	if (audio_start() != 0)
	{
		request_notify("VoiceIn",
			"Mic error: cannot open input stream");
		return ;
	}
	set_state(STATE_RECORDING);
}

static void	rec_stop(void)
{
	pthread_t	tid;

	audio_stop();
	set_state(STATE_PROCESSING);
	if (pthread_create(&tid, NULL, transcribe_thread, NULL) != 0)
	{
		perror("pthread_create");
		request_state(STATE_IDLE);
		return ;
	}
	pthread_detach(tid);
}

/* -------------------------------------------------- */
/*              GTK tray handlers                     */
/* -------------------------------------------------- */

static void	on_activate(GtkStatusIcon *icon, gpointer data)
{
	t_app_state	s;

	(void)icon;
	(void)data;
	s = atomic_load(&g_app.state);
	if (s == STATE_IDLE)
		rec_start();
	else if (s == STATE_RECORDING)
		rec_stop();
}

static void	on_toggle(GtkMenuItem *item, gpointer data)
{
	(void)item;
	(void)data;
	on_activate(NULL, NULL);
}

static void	on_quit(GtkMenuItem *item, gpointer data)
{
	(void)item;
	(void)data;
	gtk_main_quit();
}

static void	on_popup(GtkStatusIcon *icon, guint btn,
			guint time, gpointer data)
{
	(void)icon;
	(void)btn;
	(void)time;
	(void)data;
	gtk_menu_popup_at_pointer(GTK_MENU(g_app.menu), NULL);
}

/* -------------------------------------------------- */
/*              Initialization                        */
/* -------------------------------------------------- */

static int	init_whisper(void)
{
	const char			*model;
	struct whisper_context_params	cp;

	model = getenv("VOICE_IN_MODEL");
	if (!model || !*model)
		model = DEFAULT_MODEL;
	fprintf(stderr, "loading whisper model: %s\n", model);
	cp = whisper_context_default_params();
	cp.use_gpu = true;
	g_app.wctx = whisper_init_from_file_with_params(model, cp);
	if (!g_app.wctx)
	{
		fprintf(stderr, "failed to load model: %s\n", model);
		return (-1);
	}
	fprintf(stderr, "whisper ready\n");
	return (0);
}

static void	init_lang(void)
{
	const char	*lang;

	lang = getenv("VOICE_IN_LANGUAGE");
	if (!lang || !*lang)
		lang = DEFAULT_LANG;
	strncpy(g_app.lang, lang, sizeof(g_app.lang) - 1);
	g_app.lang[sizeof(g_app.lang) - 1] = '\0';
}

static void	init_device(void)
{
	const char	*dev;

	dev = getenv("VOICE_IN_DEVICE");
	g_app.input_device = (dev && *dev) ? atoi(dev) : -1;
}

static void	init_options(void)
{
	const char	*cmds;

	cmds = getenv("VOICE_IN_COMMANDS");
	g_app.commands_on = (cmds && strcmp(cmds, "1") == 0);
	fprintf(stderr, "voice commands: %s\n",
		g_app.commands_on ? "enabled" : "disabled");
}

static void	build_menu(void)
{
	GtkWidget	*toggle;
	GtkWidget	*quit;

	g_app.menu = gtk_menu_new();
	toggle = gtk_menu_item_new_with_label("Toggle recording");
	quit = gtk_menu_item_new_with_label("Quit");
	g_signal_connect(toggle, "activate",
		G_CALLBACK(on_toggle), NULL);
	g_signal_connect(quit, "activate",
		G_CALLBACK(on_quit), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(g_app.menu), toggle);
	gtk_menu_shell_append(GTK_MENU_SHELL(g_app.menu), quit);
	gtk_widget_show_all(g_app.menu);
}

static void	build_tray(void)
{
	g_app.icon_idle = make_icon(1.0, 1.0, 1.0, FALSE);
	g_app.icon_recording = make_icon(0.94, 0.33, 0.31, TRUE);
	g_app.icon_processing = make_icon(1.0, 0.65, 0.15, TRUE);
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

/* -------------------------------------------------- */
/*                  Main                              */
/* -------------------------------------------------- */

int	main(int argc, char **argv)
{
	PaError	err;

	gtk_init(&argc, &argv);
	if (!notify_init("VoiceIn"))
	{
		fprintf(stderr, "notify_init failed\n");
		return (1);
	}
	init_lang();
	init_device();
	init_options();
	g_app.audio_buf = calloc(AUDIO_CAPACITY, sizeof(float));
	if (!g_app.audio_buf)
	{
		fprintf(stderr, "cannot allocate audio buffer\n");
		return (1);
	}
	atomic_store(&g_app.audio_len, 0);
	atomic_store(&g_app.state, STATE_IDLE);
	err = Pa_Initialize();
	if (err != paNoError)
	{
		fprintf(stderr, "Pa_Initialize: %s\n",
			Pa_GetErrorText(err));
		return (1);
	}
	if (init_whisper() != 0)
	{
		Pa_Terminate();
		return (1);
	}
	build_menu();
	build_tray();
	gtk_main();
	if (g_app.stream)
		audio_stop();
	Pa_Terminate();
	whisper_free(g_app.wctx);
	notify_uninit();
	free(g_app.audio_buf);
	g_app.audio_buf = NULL;
	g_object_unref(g_app.icon_idle);
	g_object_unref(g_app.icon_recording);
	g_object_unref(g_app.icon_processing);
	return (0);
}
