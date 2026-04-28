# opons-voxd

> Note: opons-voxd is unrelated to jakovius/voxd. The name was chosen independently — `opons` = O. Pons (author), `voxd` = vox + daemon (Latin/Unix tradition).

**Local speech-to-text dictation for Linux — your voice never leaves your machine.**

🌍 **Translations:**
[Français](docs/README.fr.md) · [Deutsch](docs/README.de.md) · [中文](docs/README.zh.md) · [日本語](docs/README.ja.md) · [Español](docs/README.es.md) · [Italiano](docs/README.it.md)

---

## Why?

I wanted to dictate text into any application on Linux without sending my voice to a third-party server. Every existing solution I found — browser extensions, cloud APIs, SaaS platforms — streams your audio to remote servers for transcription. That means every word you speak transits through the internet: emails, confidential documents, client data, personal notes — all of it.

This project is a lightweight, fully local alternative. It runs entirely on your machine using [whisper.cpp](https://github.com/ggerganov/whisper.cpp), OpenAI's open-source speech recognition model. No network connection is needed after the initial setup. No data ever leaves your computer.

Built with the help of AI.

---

## How It Works

Two ways to dictate, available simultaneously:

### Tray icon mode — toggle, copy to clipboard

1. A small icon appears in your system tray
2. **Left-click** → recording starts (icon turns red)
3. **Left-click again** → recording stops, audio is transcribed locally via Whisper
4. The transcribed text is automatically copied to your clipboard (both X11 selections)
5. A desktop notification displays the text for 10 seconds
6. Paste anywhere with `Ctrl+Shift+V`, `Shift+Insert`, or middle-click

**Right-click** the icon for a menu with Toggle / Quit.

### Push-to-talk mode — hold a hotkey, type at the cursor

1. **Hold `Ctrl+Shift+Space`** → recording starts (icon turns red)
2. **Release the keys** → recording stops, audio is transcribed
3. The transcribed text is **typed directly at the keyboard cursor** (via the X11 XTest extension) into whichever window has focus

The hotkey is configurable via `OPONS_VOXD_PTT_HOTKEY` (see [Configuration](#configuration)).

### Screenshots

| Idle | Recording |
|:---:|:---:|
| ![Idle](screenshots/opons_voxd_inactive.png) | ![Recording](screenshots/opons_voxd_active.png) |

---

> ⚠️ **NVIDIA GPU with CUDA is critical for usable response times.**
> Without CUDA, transcription takes **10–15 seconds** for 15 seconds of speech (CPU only).
> With CUDA enabled, the same transcription takes **less than 1 second**.
> If you have an NVIDIA GPU (GTX 1060 or newer), setting up CUDA should be your top priority.
> See the [NVIDIA GPU](#nvidia-gpu-optional-but-critical-for-performance) section below.

---

## Features

- **100% local** — audio is processed on your CPU or GPU, never sent anywhere
- **GPU accelerated** — NVIDIA CUDA support for near-instant transcription
- **99 languages** — powered by OpenAI Whisper (French, English, German, Chinese, Japanese, Spanish, Italian, and many more)
- **Lightweight** — single C binary (~100 KB), no Python, no runtime dependencies
- **System tray integration** — unobtrusive icon in your taskbar
- **Push-to-talk hotkey** — hold `Ctrl+Shift+Space` (configurable) to record, release to type the transcript at the keyboard cursor
- **Dual clipboard** — text is pushed to both PRIMARY and CLIPBOARD X11 selections
- **Desktop notifications** — transcribed text displayed as a notification
- **Voice commands** — built-in French commands for punctuation and formatting, disabled by default (`OPONS_VOXD_COMMANDS=1` to enable, see [Voice Commands](#voice-commands))
- **Auto-capitalization** — sentences are capitalized automatically
- **Auto-start** — can be configured to launch at login

---

## Prerequisites

### Operating System

| Requirement | Minimum | Check |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Linux Kernel | 5.15+ | `uname -r` |
| Display server | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland is not supported (GtkStatusIcon requires X11). Linux Mint uses X11 by default.

### Build Tools (required)

| Package | Minimum | Check | Install |
|---|---|---|---|
| gcc | 9.0+ | `gcc --version` | `sudo apt install build-essential` |
| cmake | 3.14+ | `cmake --version` | `sudo apt install cmake` |
| pkg-config | 0.29+ | `pkg-config --version` | `sudo apt install pkg-config` |
| git | 2.25+ | `git --version` | `sudo apt install git` |

### Development Libraries (required)

| Package | Minimum | Check | Install |
|---|---|---|---|
| libgtk-3-dev | 3.22+ | `pkg-config --modversion gtk+-3.0` | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `pkg-config --modversion libnotify` | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `dpkg -s libportaudio2 \| grep Version` | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `pkg-config --modversion cairo` | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `pkg-config --modversion x11` | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `pkg-config --modversion xtst` | `sudo apt install libxtst-dev` |

> **Note on PortAudio:** we use the runtime package `libportaudio2` rather than `libportaudio-dev`. On multiarch Debian/Ubuntu systems running wine, installing `libportaudio-dev` can force the removal of i386 packages (`libasound2-plugins:i386`, `libjack-jackd2-0:i386`, `wine-devel`, …). The build vendors `portaudio.h` (fetched once via `curl` on first `make`) and links directly against `libportaudio.so.2`, so the dev package is never needed.

### Runtime Tools (required)

| Tool | Purpose | Install |
|---|---|---|
| xclip | Copy to X11 clipboards (tray mode) | `sudo apt install xclip` |
| notify-send | Desktop notifications | `sudo apt install libnotify-bin` |

Push-to-talk typing uses the XTest extension directly (provided by `libxtst6`, pulled in by `libxtst-dev` at build time). No external `xdotool` process is spawned at runtime.

### NVIDIA GPU (optional but critical for performance)

Without CUDA, expect **10–15 seconds** of processing per 15 seconds of speech. With CUDA, expect **under 1 second**. This is a **10–50x** difference.

| Component | Minimum | Check |
|---|---|---|
| NVIDIA Driver | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

If `nvidia-smi` works but `nvcc` does not, install the CUDA toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

If the CUDA version shown by `nvidia-smi` is **lower** than the one shown by `nvcc --version`, your driver is too old. Update it:

```bash
ubuntu-drivers devices                  # list available drivers
sudo apt install nvidia-driver-590      # install the latest
sudo reboot
```

### CPU

| Requirement | Check |
|---|---|
| x86_64 architecture | `uname -m` |
| AVX2 support (recommended) | `grep -o avx2 /proc/cpuinfo \| head -1` |

---

## Installation

### Step 1 — Install system dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin
```

### Step 2 — Clone this repository

```bash
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd
```

### Step 3 — Build whisper.cpp and download the model

```bash
make setup
```

This will:
1. Clone [whisper.cpp](https://github.com/ggerganov/whisper.cpp)
2. Auto-detect CUDA (if available)
3. Compile whisper.cpp (3–10 min depending on your machine)
4. Download the `medium` model (~1.5 GB) from Hugging Face

### Step 4 — Build the binary

```bash
make
```

You'll see either `[build] CUDA detected` or `[build] CUDA not found — CPU only`.

### Step 5 — Run

```bash
./opons-voxd
```

The model loads in 2–5 seconds, then the icon appears in your system tray.

To confirm GPU is active, look for `CUDA0 total size` in the output (not `CPU total size`).

---

## Configuration

All configuration is done through environment variables (all optional):

| Variable | Default | Description |
|---|---|---|
| `OPONS_VOXD_MODEL` | `whisper.cpp/models/ggml-medium.bin` | Path to the GGML model file |
| `OPONS_VOXD_LANGUAGE` | `fr` | Whisper language code, or `auto` |
| `OPONS_VOXD_DEVICE` | system default | PortAudio device index |
| `OPONS_VOXD_COMMANDS` | `0` (disabled) | Set to `1` to enable voice commands |
| `OPONS_VOXD_CMDS_FILE` | `commands/<lang>.txt` | Explicit path to a commands file |
| `OPONS_VOXD_NOTIFY_PERSIST` | `0` (transient) | Set to `1` to keep notifications in history |
| `OPONS_VOXD_PTT_HOTKEY` | `ctrl+shift+space` | Push-to-talk hotkey (e.g. `super+space`, `ctrl+alt+f1`) |

Examples:

```bash
# English dictation, voice commands enabled, persistent notifications
OPONS_VOXD_LANGUAGE=en OPONS_VOXD_COMMANDS=1 OPONS_VOXD_NOTIFY_PERSIST=1 ./opons-voxd

# Custom commands file
OPONS_VOXD_COMMANDS=1 OPONS_VOXD_CMDS_FILE=~/my_commands.txt ./opons-voxd
```

### Notifications

By default, notifications are **transient**: they appear for 10 seconds then vanish completely, leaving no trace in the notification center. Set `OPONS_VOXD_NOTIFY_PERSIST=1` if you prefer notifications to remain in the history.

### Push-to-talk hotkey

Hold the configured hotkey to record, release to type the transcript at the keyboard cursor. The default is `ctrl+shift+space`. Override it with `OPONS_VOXD_PTT_HOTKEY`:

```bash
OPONS_VOXD_PTT_HOTKEY=super+space ./opons-voxd
OPONS_VOXD_PTT_HOTKEY=ctrl+alt+f1 ./opons-voxd
```

**Format**: `mod+mod+...+key`, separated by `+`, case-insensitive.

- Modifiers: `ctrl`, `shift`, `alt`, `super`
- Key: any X11 keysym name (`space`, `f1`, `a`, `Return`, …)

If the hotkey is invalid or already grabbed by another application, push-to-talk is silently disabled and the tray icon mode keeps working normally.

### Voice Commands

Voice commands are **disabled by default** to avoid unexpected replacements (e.g., saying "period" when you actually mean the word "period"). Enable them with `OPONS_VOXD_COMMANDS=1`.

When enabled, the program loads a command file from the `commands/` directory based on the first two characters of `OPONS_VOXD_LANGUAGE`. For example, `OPONS_VOXD_LANGUAGE=fr` loads `commands/fr.txt`, `OPONS_VOXD_LANGUAGE=en` loads `commands/en.txt`.

Included command files:

| File | Language | Example commands |
|---|---|---|
| `commands/en.txt` | English | "period" → `.`, "new line" → line break |
| `commands/fr.txt` | French | "point" → `.`, "nouvelle ligne" → line break |
| `commands/de.txt` | German | "punkt" → `.`, "neue zeile" → line break |
| `commands/es.txt` | Spanish | "punto" → `.`, "nueva linea" → line break |
| `commands/it.txt` | Italian | "punto" → `.`, "nuova riga" → line break |
| `commands/zh.txt` | Chinese | "句号" → `。`, "换行" → line break |
| `commands/ja.txt` | Japanese | "句点" → `。`, "改行" → line break |

### Adding a New Language

Create a text file `commands/xx.txt` (where `xx` is your language code):

```
# One command per line: spoken_form|replacement
# Use \n for newline, \t for tab
# Longer phrases must come before shorter ones

new paragraph|\n\n
new line|\n
semicolon|;
period|.
comma|,
```

No recompilation needed — just set `OPONS_VOXD_LANGUAGE=xx` and `OPONS_VOXD_COMMANDS=1`.

### Available Models

```bash
cd whisper.cpp/models
bash ./download-ggml-model.sh small           # fast, lightweight (~466 MB)
bash ./download-ggml-model.sh large-v3-turbo  # best quality/speed with GPU (~1.6 GB)
cd ../..
```

| Model | Size | GPU | CPU (8 cores) | Quality |
|---|---|---|---|---|
| `tiny` | 75 MB | instant | ~2 s | fair |
| `small` | 466 MB | instant | ~5 s | good |
| `medium` | 1.5 GB | < 1 s | ~15 s | very good |
| `large-v3-turbo` | 1.6 GB | < 1 s | ~10 s | excellent |

> GPU timings measured on RTX 4070. CPU timings on i5-13400F (10 cores, AVX2).

---

## Auto-Start at Login

### Using the included launcher (with rotating logs)

```bash
mkdir -p ~/.config/autostart

cat > ~/.config/autostart/opons-voxd.desktop << 'EOF'
[Desktop Entry]
Type=Application
Name=opons-voxd
Comment=Local speech-to-text dictation
Exec=/path/to/opons-voxd/launch.sh
Path=/path/to/opons-voxd
Icon=audio-input-microphone
Terminal=false
StartupNotify=false
X-GNOME-Autostart-enabled=true
X-GNOME-Autostart-Delay=5
EOF
```

Replace `/path/to/opons-voxd` with your actual installation path.

The `launch.sh` script handles log rotation: `opons_voxd.log` is capped at 5 MB, with one `.old` backup.

You can also manage this through **Menu → Preferences → Startup Applications** in Cinnamon/MATE.

---

## Troubleshooting

### Icon doesn't appear in the tray

Cinnamon requires the **System Tray** applet. Right-click the panel → Applets → enable "System Tray" or "Xapp Status Applet".

### ALSA/JACK warnings at startup

Messages like `Unknown PCM cards.pcm.rear` or `jack server is not running` are **normal and harmless**. PortAudio probes all audio backends at initialization.

### `CUDA driver version is insufficient for CUDA runtime version`

Your NVIDIA driver is too old for the installed CUDA toolkit:

```bash
nvidia-smi     # shows max CUDA version supported by driver
nvcc --version # shows toolkit version
```

The `nvidia-smi` version must be ≥ `nvcc` version. Update your driver:

```bash
ubuntu-drivers devices           # list available drivers
sudo apt install nvidia-driver-590  # install latest
sudo reboot
```

### Rollback NVIDIA driver (black screen after update)

```bash
# Ctrl+Alt+F3 for a text terminal
sudo apt remove --purge nvidia-driver-590
sudo apt install nvidia-driver-565   # your previous version
sudo reboot
```

---

## Verifying Network Isolation

```bash
# While opons-voxd is running:
strace -e trace=network -fp $(pgrep opons-voxd) 2>&1 | grep -v getsockopt
# No connect() or sendto() will appear.

ss -tnp | grep opons-voxd
# No TCP connections.
```

The only subprocesses launched are `xclip` (local clipboard) and `notify-send` (desktop notification). Neither makes network connections.

---

## What Is Whisper?

[Whisper](https://github.com/openai/whisper) is a speech recognition model by OpenAI, trained on 680,000 hours of multilingual audio. It supports 99 languages.

This project uses [whisper.cpp](https://github.com/ggerganov/whisper.cpp), a C/C++ reimplementation by Georgi Gerganov (creator of [llama.cpp](https://github.com/ggerganov/llama.cpp)). No Python or PyTorch required.

### Licenses — everything is MIT

| Component | Author | License | Repository |
|---|---|---|---|
| Whisper model (weights) | OpenAI | MIT | [openai/whisper](https://github.com/openai/whisper) |
| whisper.cpp (inference engine) | Georgi Gerganov | MIT | [ggerganov/whisper.cpp](https://github.com/ggerganov/whisper.cpp) |
| opons-voxd (this project) | Olivier Pons | MIT | [olivierpons/opons-voxd](https://github.com/olivierpons/opons-voxd) |

---

## Project Structure

| File | Purpose |
|---|---|
| `opons_voxd.c` | Main source (~950 lines) |
| `Makefile` | Build system with automatic CUDA detection |
| `launch.sh` | Launcher with rotating logs for auto-start |
| `commands/*.txt` | Voice command files (en, fr, de, es, it, zh, ja) |
| `screenshots/` | Tray icon screenshots |
| `README.md` | This document |
| `docs/` | Translated documentation |

---

## Coding Style

This project follows the **Linux kernel coding style** (`Documentation/process/coding-style.rst`) with minor adaptations by Olivier Pons:

- SPDX license identifier on line 1
- K&R braces for control structures, new line for function bodies
- 4-space indentation (no tabs)
- `struct name` directly, no typedefs for structures
- No artificial prefixes (`s_`, `t_`, `e_`)
- `return value;` without parentheses
- `sizeof(*ptr)` instead of `sizeof(struct type)`
- kernel-doc `/** */` comments for non-trivial functions
- 80 characters per line maximum, strictly enforced
- One variable per line, `const` wherever possible
- Early returns to reduce nesting depth
