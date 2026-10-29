# opons-voxd

> Hinweis: opons-voxd hat nichts mit jakovius/voxd zu tun. Der Name wurde unabhängig davon gewählt: `opons` = O. Pons (Autor), `voxd` = vox + daemon (lateinische und Unix-Tradition).

> **⚠ Abwärtskompatibilität bewusst aufgegeben.** Dieses Projekt hieß früher `voice_in_linux`. Die Umbenennung betraf den Namen der Binärdatei, den Namen der Quelldatei, sämtliche Umgebungsvariablen (`VOICE_IN_*` → `OPONS_VOXD_*`) sowie die Laufzeitabhängigkeit von `xdotool` (ersetzt durch direkte XTest-Synthese über `libxtst-dev`). Keine Aliase, keine Übergangsfrist – passen Sie Ihre Startskripte, Autostart-Einträge und `apt install`-Zeilen an.

**Lokales Diktieren für Linux – Ihre Stimme verlässt nie Ihren Rechner.**

🌍 **Übersetzungen:**
[English](../README.md) · [Français](README.fr.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Mein Workflow: in der eigenen Sprache diktieren, auf Englisch tippen](WORKFLOW.de.md)** – ein Anwendungsfall aus der Praxis

---

## Warum?

Ich wollte unter Linux in jede beliebige Anwendung diktieren, ohne meine Stimme an einen fremden Server zu schicken. Alle Lösungen, die ich gefunden habe – Browser-Erweiterungen, Cloud-APIs, SaaS-Plattformen –, schicken Ihr Audio zur Transkription an entfernte Server. Jedes gesprochene Wort geht also durchs Internet: E-Mails, vertrauliche Dokumente, Kundendaten, persönliche Notizen – einfach alles.

Dieses Projekt ist eine schlanke, vollständig lokale Alternative. Es läuft komplett auf Ihrem Rechner, mit [whisper.cpp](https://github.com/ggerganov/whisper.cpp) und Whisper, dem quelloffenen Spracherkennungsmodell von OpenAI. Nach der Ersteinrichtung ist keine Netzwerkverbindung mehr nötig. Keine Daten verlassen jemals Ihren Computer.

Mit Unterstützung von KI entwickelt.

---

## Funktionsweise

Zwei Arten zu diktieren, beide gleichzeitig verfügbar.

### Tray-Modus – Umschalten, Kopieren in die Zwischenablage

1. Im Infobereich der Taskleiste (System-Tray) erscheint ein kleines Symbol
2. **Linksklick** → die Aufnahme startet (das Symbol wird rot)
3. **Erneuter Linksklick** → die Aufnahme stoppt, das Audio wird lokal mit Whisper transkribiert
4. Der transkribierte Text wird automatisch in die Zwischenablage kopiert (in beide X11-Auswahlen)
5. Eine Desktop-Benachrichtigung zeigt den Text 10 Sekunden lang an
6. Fügen Sie ihn überall mit `Ctrl+Shift+V`, `Shift+Insert` oder einem Mittelklick ein

Ein **Rechtsklick** auf das Symbol öffnet ein Menü mit Toggle / Unload models / Reload models / Quit (Umschalten, Modelle entladen, Modelle neu laden, Beenden). Siehe [GPU für lokale KI freigeben](#gpu-für-lokale-ki-freigeben-mcp-server).

### Push-to-Talk-Modus – Tastenkürzel halten, Eingabe am Cursor

1. **`Ctrl+Shift+Space` gedrückt halten** → die Aufnahme startet (das Symbol wird rot)
2. **Tasten loslassen** → die Aufnahme stoppt, das Audio wird transkribiert
3. Der transkribierte Text wird **direkt am Tastaturcursor eingetippt** (über die X11-Erweiterung XTest), und zwar in das Fenster, das gerade den Fokus hat

Das Tastenkürzel lässt sich über `OPONS_VOXD_PTT_HOTKEY` einstellen. Ein zweites, unabhängiges Tastenkürzel (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, Standard `ctrl+alt+t`) funktioniert genauso, tippt aber stattdessen eine englische Übersetzung – siehe [Haupt-README](../README.md#push-to-talk-and-translate-hotkey).

### Screenshots

| Leerlauf | Aufnahme |
|:---:|:---:|
| ![Symbol im Leerlauf](../screenshots/opons_voxd_inactive.png) | ![Symbol während der Aufnahme](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Für brauchbare Antwortzeiten ist eine NVIDIA-GPU mit CUDA entscheidend.**
> Ohne CUDA dauert die Transkription von 15 Sekunden Sprache **10–15 Sekunden** (nur CPU).
> Mit CUDA dauert dieselbe Transkription **weniger als 1 Sekunde**.

---

## Funktionen

- **100 % lokal** – das Audio wird auf Ihrer CPU oder GPU verarbeitet und nirgendwohin gesendet
- **GPU-beschleunigt** – Unterstützung für NVIDIA CUDA, für nahezu sofortige Transkription
- **99 Sprachen** – dank OpenAI Whisper
- **Schlank** – reines C, kein Python: der Daemon plus ein optionaler MCP-Server von 25 KB, der nur gegen die libc gelinkt ist
- **Tray-Integration** – unaufdringliches Symbol in der Taskleiste
- **Push-to-Talk-Tastenkürzel** – `Ctrl+Shift+Space` (einstellbar) zum Aufnehmen gedrückt halten, loslassen, und das Transkript wird am Tastaturcursor eingetippt
- **Push-to-Talk-Tastenkürzel mit Übersetzung** – ein zweites, unabhängig einstellbares Tastenkürzel tippt eine englische Übersetzung des Gesprochenen statt einer wörtlichen Transkription
- **Doppelte Zwischenablage** – der Text landet in beiden X11-Auswahlen, PRIMARY und CLIPBOARD
- **Desktop-Benachrichtigungen** – der transkribierte Text erscheint als Benachrichtigung
- **Sprachbefehle** – eine Befehlsdatei pro Sprache in `commands/`, standardmäßig deaktiviert (aktivieren mit `OPONS_VOXD_COMMANDS=1`). Für eine neue Sprache legen Sie einfach `commands/xx.txt` an, ohne neu zu kompilieren.
- **Flüchtige Benachrichtigungen** – standardmäßig erscheinen Benachrichtigungen und verschwinden danach vollständig. `OPONS_VOXD_NOTIFY_PERSIST=1` behält sie im Verlauf.
- **Benachrichtigungsmodus** – `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` steuert die Erfolgsmeldungen (Standard `quiet`: Sprechblase, kein Ton). Fehler erscheinen immer mit Ton, in jedem Modus und ausnahmslos. Die vollständige Tabelle steht im Haupt-README.
- **Automatische Großschreibung** – Satzanfänge werden automatisch großgeschrieben
- **GPU bei Bedarf freigeben** – die Whisper-Modelle aus dem VRAM entladen (über das Tray-Menü oder durch einen beliebigen KI-Agenten über den integrierten MCP-Server) und danach wieder laden
- **Autostart** – lässt sich so einrichten, dass es beim Anmelden startet

---

## Voraussetzungen

### Betriebssystem

| Anforderung | Minimum | Prüfen |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Linux-Kernel | 5.15+ | `uname -r` |
| Displayserver | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland wird nicht unterstützt (GtkStatusIcon benötigt X11). Linux Mint verwendet standardmäßig X11.

### Build-Werkzeuge (erforderlich)

| Paket | Minimum | Installation |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Entwicklungsbibliotheken (erforderlich)

| Paket | Minimum | Installation |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` wird nicht benötigt: Der Build lädt `portaudio.h` herunter (per `curl`, beim ersten `make`) und linkt direkt gegen `libportaudio.so.2`. Details im [Haupt-README](../README.md#development-libraries-required).

### Laufzeitwerkzeuge (erforderlich)

| Werkzeug | Zweck | Installation |
|---|---|---|
| xclip | Kopieren in die X11-Zwischenablagen | `sudo apt install xclip` |
| notify-send | Desktop-Benachrichtigungen | `sudo apt install libnotify-bin` |

### NVIDIA-GPU (optional, aber entscheidend für die Leistung)

Ohne CUDA müssen Sie für 15 Sekunden Sprache mit **10–15 Sekunden** Verarbeitung rechnen. Mit CUDA sind es **weniger als 1 Sekunde**. Das ist **10- bis 50-mal** schneller.

| Komponente | Minimum | Prüfen |
|---|---|---|
| NVIDIA-Treiber | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Wenn `nvidia-smi` funktioniert, `nvcc` aber nicht, installieren Sie das CUDA Toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Wenn die von `nvidia-smi` angezeigte CUDA-Version **niedriger** ist als die von `nvcc` gemeldete, aktualisieren Sie den Treiber:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Installation

```bash
# 1. Systemabhängigkeiten
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Repository klonen
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. whisper.cpp bauen und Modell herunterladen
make setup

# 4. Binärdateien bauen: opons-voxd (der Daemon) und opons-voxd-mcp (optionaler MCP-Server)
make

# 5. Starten
./opons-voxd
```

---

## GPU für lokale KI freigeben (MCP-Server)

Die Whisper-Modelle liegen ständig im GPU-Speicher (etwa 3,8 GB für `large-v3-turbo` + `medium`). Wenn Sie diesen VRAM für etwas anderes brauchen – ein lokales LLM, ein Bildmodell, ein Training –, kann opons-voxd die Modelle entladen und danach wieder laden, ohne dass Sie das Programm beenden müssen. Beim Entladen wird außerdem der CUDA-Kontext des Prozesses zerstört, sodass opons-voxd vollständig aus `nvidia-smi` verschwindet (0 MB VRAM). Das Neuladen dauert weniger als eine Sekunde, wenn die Modelldateien noch im Page-Cache liegen, sonst einige Sekunden.

**Zustände des Tray-Symbols:**

| Symbol | Zustand |
|:---:|---|
| weißer Ring | Modelle geladen, bereit |
| roter Punkt | Aufnahme läuft |
| orangefarbener Punkt | Transkription läuft |
| grauer, durchgestrichener Ring (⊘) | Modelle entladen, GPU-Speicher freigegeben, Diktieren deaktiviert |
| blauer Dreiviertelbogen | Modelle werden neu geladen |

Solange die Modelle entladen sind, löst ein Klick auf das Symbol oder ein Push-to-Talk-Tastenkürzel statt einer Aufnahme eine Fehlermeldung aus.

**Von Hand:** Rechtsklick auf das Tray-Symbol > *Unload models (free GPU memory)* (Modelle entladen), danach *Reload models* (Modelle neu laden).

**Über einen KI-Agenten (MCP):** Starten Sie den Daemon mit `-m` / `--mcp` (standardmäßig aus). `make` baut außerdem `opons-voxd-mcp`, einen [Model Context Protocol](https://modelcontextprotocol.io)-Server über stdio, der nur gegen die libc gelinkt ist (etwa 1,5 MB RAM). Er stellt drei Werkzeuge bereit: `unload_models`, `reload_models` und `get_status`. Mit Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

Die meisten anderen Clients (Claude Desktop, Cursor, LM Studio …) verwenden dieses JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Alles Weitere im [Haupt-README](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Push-to-Talk-Tastenkürzel

Das mitgelieferte `launch.sh` verwendet `ctrl+alt+w` (empfohlen bei AZERTY) oder `ctrl+alt+z` (empfohlen bei QWERTY). Die Taste liegt neben `A`: Die ganze Kombination erreichen Sie allein mit der linken Hand (kleiner Finger auf `Ctrl`, Daumen auf `Alt`, Ring- oder Mittelfinger auf `W`/`Z`), ohne Ihre Schreibhaltung aufzugeben. Außerdem ist sie in den meisten Desktop-Umgebungen standardmäßig nicht belegt. Das Tastenkürzel lässt sich über `OPONS_VOXD_PTT_HOTKEY` einstellen. Alles Weitere im [Haupt-README](../README.md#push-to-talk-hotkey).

---

## Lizenz

Alles steht unter der MIT-Lizenz: das Whisper-Modell (OpenAI), whisper.cpp (Georgi Gerganov) und dieses Projekt.

Vollständige Dokumentation: siehe [Haupt-README](../README.md).

---

## Codierstil

Dieses Projekt folgt dem **Coding-Style des Linux-Kernels** (`Documentation/process/coding-style.rst`) mit Anpassungen von Olivier Pons: K&R-Klammern, Einrückung mit 4 Leerzeichen, `struct name` ohne typedef, keine Präfixe (`s_`, `t_`, `e_`), `return value;` ohne Klammern, kernel-doc-Kommentare, höchstens 80 Zeichen pro Zeile. Alles Weitere im [Haupt-README](../README.md).
