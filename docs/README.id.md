# opons-voxd

> Catatan: opons-voxd tidak berkaitan dengan jakovius/voxd. Namanya dipilih secara terpisah — `opons` = O. Pons (penulis), `voxd` = vox + daemon (tradisi Latin/Unix).

> **⚠ Kompatibilitas mundur sengaja diputus.** Proyek ini sebelumnya bernama `voice_in_linux`. Penggantian nama ini mengubah nama berkas biner, nama berkas sumber, semua variabel lingkungan (`VOICE_IN_*` → `OPONS_VOXD_*`), serta dependensi runtime pada `xdotool` (kini digantikan oleh sintesis XTest langsung melalui `libxtst-dev`). Tanpa alias, tanpa masa transisi — perbarui skrip peluncur, entri autostart, dan baris `apt install` Anda.

**Dikte suara-ke-teks lokal untuk Linux — suara Anda tidak pernah keluar dari komputer Anda.**

🌍 **Terjemahan:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Alur Kerja Saya: Berbicara dalam Bahasa Sendiri, Diketik dalam Bahasa Inggris](WORKFLOW.id.md)** — contoh penggunaan nyata

---

## Mengapa?

Saya ingin mendikte teks ke aplikasi apa pun di Linux tanpa mengirim suara saya ke server pihak ketiga. Semua solusi yang saya temukan — ekstensi peramban, API cloud, platform SaaS — mengalirkan audio Anda ke server jarak jauh untuk ditranskripsi. Artinya, setiap kata yang Anda ucapkan melintasi internet: email, dokumen rahasia, data klien, catatan pribadi — semuanya.

Proyek ini adalah alternatif yang ringan dan sepenuhnya lokal. Seluruh prosesnya berjalan di komputer Anda menggunakan [whisper.cpp](https://github.com/ggerganov/whisper.cpp), implementasi Whisper, model pengenalan suara sumber terbuka dari OpenAI. Setelah penyiapan awal, koneksi jaringan tidak diperlukan lagi. Tidak ada data yang keluar dari komputer Anda.

Dibuat dengan bantuan AI.

---

## Cara kerja

Ada dua cara mendikte, dan keduanya dapat dipakai bersamaan.

### Mode ikon tray — aktif/nonaktif, salin ke clipboard

1. Ikon kecil muncul di system tray
2. **Klik kiri** → perekaman dimulai (ikon berubah merah)
3. **Klik kiri lagi** → perekaman berhenti, audio ditranskripsi secara lokal oleh Whisper
4. Teks hasil transkripsi otomatis disalin ke clipboard (ke kedua seleksi X11)
5. Notifikasi desktop menampilkan teks tersebut selama 10 detik
6. Tempel di mana saja dengan `Ctrl+Shift+V`, `Shift+Insert`, atau klik tengah

**Klik kanan** ikon untuk membuka menu berisi Toggle / Unload models / Reload models / Quit (lihat [Membebaskan GPU untuk AI lokal](#membebaskan-gpu-untuk-ai-lokal-server-mcp)).

### Mode push-to-talk — tahan hotkey, teks diketik di kursor

1. **Tahan `Ctrl+Shift+Space`** → perekaman dimulai (ikon berubah merah)
2. **Lepaskan tombol** → perekaman berhenti, audio ditranskripsi
3. Teks hasil transkripsi **langsung diketik di posisi kursor keyboard** (melalui ekstensi X11 XTest) di jendela yang sedang aktif

Hotkey dapat diatur melalui `OPONS_VOXD_PTT_HOTKEY`. Hotkey kedua yang independen (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, bawaan `ctrl+alt+t`) bekerja dengan cara yang sama, tetapi mengetikkan terjemahan bahasa Inggris — lihat [README utama](../README.md#push-to-talk-and-translate-hotkey).

### Tangkapan layar

| Siaga | Merekam |
|:---:|:---:|
| ![Ikon saat siaga](../screenshots/opons_voxd_inactive.png) | ![Ikon saat merekam](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **GPU NVIDIA dengan CUDA sangat penting agar waktu respons layak dipakai.**
> Tanpa CUDA, transkripsi 15 detik ucapan memakan waktu **10–15 detik** (hanya CPU).
> Dengan CUDA aktif, transkripsi yang sama memakan waktu **kurang dari 1 detik**.

---

## Fitur

- **100% lokal** — audio diproses di CPU atau GPU Anda, tidak pernah dikirim ke mana pun
- **Akselerasi GPU** — dukungan NVIDIA CUDA untuk transkripsi yang nyaris instan
- **99 bahasa** — didukung oleh OpenAI Whisper
- **Ringan** — C murni, tanpa Python: daemon, ditambah server MCP opsional berukuran 25 KB yang hanya ditautkan ke libc
- **Integrasi system tray** — ikon yang tidak mencolok di taskbar
- **Hotkey push-to-talk** — tahan `Ctrl+Shift+Space` (dapat diatur) untuk merekam, lalu lepaskan agar transkripsi diketik di posisi kursor keyboard
- **Hotkey push-to-talk dengan terjemahan** — hotkey kedua yang dapat diatur secara terpisah, yang mengetikkan terjemahan bahasa Inggris dari ucapan Anda alih-alih transkripsi kata demi kata
- **Clipboard ganda** — teks dikirim ke kedua seleksi X11, PRIMARY dan CLIPBOARD
- **Notifikasi desktop** — teks hasil transkripsi ditampilkan sebagai notifikasi
- **Perintah suara** — satu berkas perintah per bahasa di `commands/`, nonaktif secara bawaan (aktifkan dengan `OPONS_VOXD_COMMANDS=1`). Untuk menambah bahasa, cukup buat `commands/xx.txt`, tanpa perlu kompilasi ulang.
- **Notifikasi sementara** — secara bawaan, notifikasi muncul lalu hilang sepenuhnya. `OPONS_VOXD_NOTIFY_PERSIST=1` menyimpannya di riwayat.
- **Mode notifikasi** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` mengatur notifikasi keberhasilan (bawaan `quiet`: balon, tanpa suara). Galat selalu ditampilkan dengan suara, di semua mode, tanpa kecuali. Tabel lengkapnya ada di README utama.
- **Huruf kapital otomatis** — awal kalimat otomatis diberi huruf kapital
- **Pembebasan GPU sesuai permintaan** — keluarkan model Whisper dari VRAM (lewat menu tray, atau lewat agen AI apa pun melalui server MCP bawaan) dan muat kembali setelah selesai
- **Mulai otomatis** — dapat diatur agar berjalan saat login

---

## Prasyarat

### Sistem operasi

| Persyaratan | Minimum | Pemeriksaan |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Kernel Linux | 5.15+ | `uname -r` |
| Server tampilan | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland tidak didukung (GtkStatusIcon memerlukan X11). Linux Mint memakai X11 secara bawaan.

### Alat build (wajib)

| Paket | Minimum | Instalasi |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Pustaka pengembangan (wajib)

| Paket | Minimum | Instalasi |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` tidak diperlukan: proses build mengunduh `portaudio.h` (melalui `curl`, saat `make` pertama) dan menautkan langsung ke `libportaudio.so.2`. Detailnya ada di [README utama](../README.md#development-libraries-required).

### Alat runtime (wajib)

| Alat | Fungsi | Instalasi |
|---|---|---|
| xclip | Menyalin ke clipboard X11 | `sudo apt install xclip` |
| notify-send | Notifikasi desktop | `sudo apt install libnotify-bin` |

### GPU NVIDIA (opsional, tetapi penting untuk performa)

Tanpa CUDA, pemrosesan 15 detik ucapan memakan waktu **10–15 detik**. Dengan CUDA, **kurang dari 1 detik**. Artinya, ada percepatan **10–50 kali lipat**.

| Komponen | Minimum | Pemeriksaan |
|---|---|---|
| Driver NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Jika `nvidia-smi` berjalan tetapi `nvcc` tidak, pasang CUDA Toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Jika versi CUDA yang ditampilkan `nvidia-smi` **lebih rendah** daripada yang dilaporkan `nvcc`, perbarui driver:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Instalasi

```bash
# 1. Dependensi sistem
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Kloning repositori
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Build whisper.cpp dan unduh model
make setup

# 4. Build biner: opons-voxd (daemon) dan opons-voxd-mcp (server MCP opsional)
make

# 5. Jalankan
./opons-voxd
```

---

## Membebaskan GPU untuk AI lokal (server MCP)

Model Whisper selalu berada di memori GPU (sekitar 3,8 GB untuk `large-v3-turbo` + `medium`). Saat Anda membutuhkan VRAM itu untuk keperluan lain — LLM lokal, model gambar, proses pelatihan — opons-voxd dapat mengeluarkan model-model tersebut dari memori lalu memuatnya kembali, tanpa perlu ditutup. Mengeluarkan model juga menghapus konteks CUDA milik proses, sehingga opons-voxd hilang sepenuhnya dari `nvidia-smi` (0 MB VRAM). Memuat ulang butuh waktu kurang dari satu detik jika berkas model masih ada di page cache, dan beberapa detik jika tidak.

**Status ikon tray:**

| Ikon | Status |
|:---:|---|
| cincin putih | model termuat, siap |
| cakram merah | merekam |
| cakram oranye | mentranskripsi |
| cincin abu-abu bergaris miring (⊘) | model dikeluarkan, memori GPU dibebaskan, dikte nonaktif |
| busur biru tiga perempat lingkaran | model sedang dimuat ulang |

Selama model dikeluarkan, mengeklik ikon atau menekan hotkey push-to-talk akan menampilkan notifikasi galat alih-alih memulai perekaman.

**Secara manual:** klik kanan ikon tray > *Unload models (free GPU memory)* (keluarkan model), lalu *Reload models* (muat ulang model).

**Dari agen AI (MCP):** jalankan daemon dengan `-m` / `--mcp` (nonaktif secara bawaan). `make` juga membangun `opons-voxd-mcp`, server [Model Context Protocol](https://modelcontextprotocol.io) melalui stdio yang hanya ditautkan ke libc (sekitar 1,5 MB RAM). Server ini menyediakan tiga alat: `unload_models`, `reload_models`, dan `get_status`. Dengan Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

Sebagian besar klien lain (Claude Desktop, Cursor, LM Studio...) memakai JSON berikut:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Detail lengkapnya ada di [README utama](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Hotkey push-to-talk

`launch.sh` yang disertakan memakai `ctrl+alt+w` (disarankan untuk AZERTY) atau `ctrl+alt+z` (disarankan untuk QWERTY). Tombol tersebut terletak di sebelah `A`, sehingga seluruh kombinasi dapat dijangkau dengan tangan kiri saja (kelingking di `Ctrl`, ibu jari di `Alt`, jari manis atau jari tengah di `W`/`Z`) tanpa mengubah posisi mengetik. Selain itu, di sebagian besar lingkungan desktop kombinasi ini belum dipakai secara bawaan. Hotkey ini diatur melalui `OPONS_VOXD_PTT_HOTKEY`. Detail lengkapnya ada di [README utama](../README.md#push-to-talk-hotkey).

---

## Lisensi

Semuanya berlisensi MIT: model Whisper (OpenAI), whisper.cpp (Georgi Gerganov), dan proyek ini.

Dokumentasi lengkap: lihat [README utama](../README.md).

---

## Gaya kode

Proyek ini mengikuti **gaya kode kernel Linux** (`Documentation/process/coding-style.rst`) dengan penyesuaian oleh Olivier Pons: kurung kurawal K&R, indentasi 4 spasi, `struct name` tanpa typedef, tanpa prefiks (`s_`, `t_`, `e_`), `return value;` tanpa tanda kurung, komentar kernel-doc, maksimal 80 karakter per baris. Detail lengkapnya ada di [README utama](../README.md).
