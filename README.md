# Voice In Linux

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

1. A small icon appears in your system tray
2. **Left-click** → recording starts (icon turns red)
3. **Left-click again** → recording stops, audio is transcribed locally via Whisper
4. The transcribed text is automatically copied to your clipboard (both X11 selections)
5. A desktop notification displays the text for 10 seconds
6. Paste anywhere with `Ctrl+Shift+V`, `Shift+Insert`, or middle-click

**Right-click** the icon for a menu with Toggle / Quit.

---

## Features

- **100% local** — audio is processed on your CPU or GPU, never sent anywhere
- **GPU accelerated** — NVIDIA CUDA support for near-instant transcription
- **99 languages** — powered by OpenAI Whisper (French, English, German, Chinese, Japanese, Spanish, Italian, and many more)
- **Lightweight** — single C binary (~100 KB), no Python, no runtime dependencies
- **System tray integration** — unobtrusive icon in your taskbar
- **Dual clipboard** — text is pushed to both PRIMARY and CLIPBOARD X11 selections
- **Desktop notifications** — transcribed text displayed as a notification
- **Voice commands** — built-in French commands for punctuation ("point", "virgule", "nouvelle ligne"...)
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
| libportaudio-dev | 19.6+ | `pkg-config --modversion portaudio-2.0` | `sudo apt install libportaudio-dev` |
| libcairo2-dev | 1.14+ | `pkg-config --modversion cairo` | `sudo apt install libcairo2-dev` |

### Runtime Tools (required)

| Tool | Purpose | Install |
|---|---|---|
| xclip | Copy to X11 clipboards | `sudo apt install xclip` |
| notify-send | Desktop notifications | `sudo apt install libnotify-bin` |

### NVIDIA GPU (optional, strongly recommended)

| Component | Minimum | Check |
|---|---|---|
| NVIDIA Driver | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

> The CUDA version shown by `nvidia-smi` must be **≥** the version shown by `nvcc --version`. If not, update your driver.

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
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin
```

### Step 2 — Clone this repository

```bash
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux
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
./voice_in
```

The model loads in 2–5 seconds, then the icon appears in your system tray.

To confirm GPU is active, look for `CUDA0 total size` in the output (not `CPU total size`).

---

## Configuration

All configuration is done through environment variables (all optional):

| Variable | Default | Example |
|---|---|---|
| `VOICE_IN_MODEL` | `whisper.cpp/models/ggml-medium.bin` | `VOICE_IN_MODEL=whisper.cpp/models/ggml-small.bin ./voice_in` |
| `VOICE_IN_LANGUAGE` | `fr` | `VOICE_IN_LANGUAGE=en ./voice_in` |
| `VOICE_IN_DEVICE` | system default | `VOICE_IN_DEVICE=3 ./voice_in` |

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

cat > ~/.config/autostart/voice-in.desktop << 'EOF'
[Desktop Entry]
Type=Application
Name=VoiceIn Local
Comment=Local speech-to-text dictation
Exec=/path/to/voice_in_linux/launch.sh
Path=/path/to/voice_in_linux
Icon=audio-input-microphone
Terminal=false
StartupNotify=false
X-GNOME-Autostart-enabled=true
X-GNOME-Autostart-Delay=5
EOF
```

Replace `/path/to/voice_in_linux` with your actual installation path.

The `launch.sh` script handles log rotation: `voice_in.log` is capped at 5 MB, with one `.old` backup.

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
# While voice_in is running:
strace -e trace=network -fp $(pgrep voice_in) 2>&1 | grep -v getsockopt
# No connect() or sendto() will appear.

ss -tnp | grep voice_in
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
| voice_in_linux (this project) | Olivier Pons | MIT | [olivierpons/voice_in_linux](https://github.com/olivierpons/voice_in_linux) |

---

## Project Structure

| File | Purpose |
|---|---|
| `voice_in.c` | Main source (~600 lines) |
| `Makefile` | Build system with automatic CUDA detection |
| `launch.sh` | Launcher with rotating logs for auto-start |
| `README.md` | This document |
| `docs/` | Translated documentation |
