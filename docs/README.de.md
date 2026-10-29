# opons-voxd

> Hinweis: opons-voxd hat nichts mit jakovius/voxd zu tun. Der Name wurde unabhängig davon gewählt — `opons` = O. Pons (Autor), `voxd` = vox + Daemon (lateinische und Unix-Tradition).

> **⚠ Abwärtskompatibilität bewusst aufgegeben.** Dieses Projekt hieß früher `voice_in_linux`. Die Umbenennung betrifft den Namen der Binärdatei, die Quelldatei, sämtliche Umgebungsvariablen (`VOICE_IN_*` → `OPONS_VOXD_*`) sowie die Laufzeitabhängigkeit von `xdotool` (ersetzt durch direkte XTest-Synthese über `libxtst-dev`). Keine Aliase, keine Übergangsfrist — passen Sie Ihre Startskripte, Autostart-Einträge und `apt install`-Zeilen an.

**Lokale Sprache-zu-Text-Diktierlösung für Linux — Ihre Stimme verlässt niemals Ihren Rechner.**

🌍 **Sprachen:**
[English](../README.md) · [Français](README.fr.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md)

---

## Warum?

Ich wollte Text in jede Linux-Anwendung diktieren können, ohne meine Stimme an einen Drittanbieter-Server zu senden. Jede bestehende Lösung — Browser-Erweiterungen, Cloud-APIs, SaaS-Plattformen — streamt Ihr Audio an entfernte Server zur Transkription. Das bedeutet, dass jedes gesprochene Wort über das Internet übertragen wird: E-Mails, vertrauliche Dokumente, Kundendaten, persönliche Notizen — alles.

Dieses Projekt ist eine leichtgewichtige, vollständig lokale Alternative. Es läuft ausschließlich auf Ihrem Rechner mit [whisper.cpp](https://github.com/ggerganov/whisper.cpp), OpenAIs Open-Source-Spracherkennungsmodell. Nach der Ersteinrichtung ist keine Netzwerkverbindung erforderlich. Keine Daten verlassen jemals Ihren Computer.

Erstellt mit Hilfe von künstlicher Intelligenz.

---

## Funktionsweise

Zwei Arten zu diktieren, gleichzeitig nutzbar.

### Tray-Symbol — Umschalten, Kopieren in die Zwischenablage

1. Ein kleines Symbol erscheint im System-Tray
2. **Linksklick** → Aufnahme startet (Symbol wird rot)
3. **Erneuter Linksklick** → Aufnahme stoppt, Audio wird lokal via Whisper transkribiert
4. Der transkribierte Text wird automatisch in die Zwischenablage kopiert (beide X11-Selektionen)
5. Eine Desktop-Benachrichtigung zeigt den Text 10 Sekunden lang an
6. Einfügen mit `Ctrl+Shift+V`, `Shift+Insert` oder Mittelklick

**Rechtsklick** auf das Symbol öffnet ein Menü mit Toggle / Unload models / Reload models / Quit (siehe [GPU freigeben](#gpu-für-lokale-ki-freigeben-mcp-server)).

### Push-to-talk — Tastenkürzel halten, Text erscheint am Cursor

1. **`Ctrl+Shift+Space` gedrückt halten** → Aufnahme startet (Symbol wird rot)
2. **Tasten loslassen** → Aufnahme stoppt, Audio wird transkribiert
3. Der Text wird **direkt am Tastaturcursor eingetippt** (über die X11-Erweiterung XTest), im gerade fokussierten Fenster

Das Tastenkürzel lässt sich über `OPONS_VOXD_PTT_HOTKEY` ändern. Ein zweites, unabhängiges Kürzel (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, standardmäßig `ctrl+alt+t`) funktioniert genauso, tippt aber eine englische Übersetzung — siehe [Haupt-README](../README.md#push-to-talk-and-translate-hotkey).

### Screenshots

| Inaktiv | Aufnahme |
|:---:|:---:|
| ![Inaktiv](../screenshots/opons_voxd_inactive.png) | ![Aufnahme](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **NVIDIA-GPU mit CUDA ist entscheidend für akzeptable Antwortzeiten.**
> Ohne CUDA: **10–15 Sekunden** Verarbeitung pro 15 Sekunden Sprache.
> Mit CUDA: **unter 1 Sekunde**. Wenn Sie eine NVIDIA-GPU haben (GTX 1060 oder neuer), sollte die CUDA-Einrichtung höchste Priorität haben.

---

## Funktionen

- **100% lokal** — Audio wird auf CPU oder GPU verarbeitet, nie versendet
- **GPU-beschleunigt** — NVIDIA-CUDA-Unterstützung für nahezu sofortige Transkription
- **99 Sprachen** — unterstützt durch OpenAI Whisper
- **Leichtgewichtig** — reines C, kein Python: der Daemon plus ein optionaler, 25 KB kleiner MCP-Server, der nur gegen die libc gelinkt ist
- **System-Tray-Integration** — unauffälliges Symbol in der Taskleiste
- **Push-to-talk-Kürzel** — `Ctrl+Shift+Space` (konfigurierbar) halten zum Aufnehmen, loslassen, und der Text wird am Tastaturcursor eingetippt
- **Push-to-talk mit Übersetzung** — ein zweites, separat konfigurierbares Kürzel tippt statt der wörtlichen Transkription eine englische Übersetzung des Gesprochenen
- **Doppelte Zwischenablage** — Text in PRIMARY und CLIPBOARD
- **Desktop-Benachrichtigungen** — der transkribierte Text wird als Benachrichtigung angezeigt
- **Sprachbefehle** — Befehlsdateien pro Sprache in `commands/`, standardmäßig deaktiviert (`OPONS_VOXD_COMMANDS=1`). Neue Sprache: einfach `commands/xx.txt` erstellen, keine Neukompilierung nötig.
- **Flüchtige Benachrichtigungen** — erscheinen und verschwinden vollständig. `OPONS_VOXD_NOTIFY_PERSIST=1`, um sie im Verlauf zu behalten.
- **Benachrichtigungsmodus** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` steuert die Erfolgsmeldungen (Standard `quiet`: Sprechblase ohne Ton). Fehler werden immer angezeigt und mit Ton gemeldet, in jedem Modus, ohne Ausnahme. Die vollständige Tabelle steht im Haupt-README.
- **Automatische Großschreibung** — Satzanfänge werden automatisch großgeschrieben
- **GPU bei Bedarf freigeben** — die Whisper-Modelle aus dem VRAM entladen (über das Tray-Menü oder jeden KI-Agenten über den integrierten MCP-Server) und danach wieder laden
- **Autostart** — beim Login konfigurierbar

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

# 3. whisper.cpp kompilieren + Modell herunterladen
make setup

# 4. Binärdateien kompilieren: opons-voxd (Daemon) und opons-voxd-mcp (optionaler MCP-Server)
make

# 5. Starten
./opons-voxd
```

---

## GPU für lokale KI freigeben (MCP-Server)

Die Whisper-Modelle belegen dauerhaft GPU-Speicher (etwa 3,8 GB für `large-v3-turbo` + `medium`). Brauchen Sie diesen VRAM für etwas anderes — ein lokales LLM, ein Bildmodell, ein Training —, kann opons-voxd die Modelle entladen und später wieder laden, ohne beendet zu werden. Beim Entladen wird auch der CUDA-Kontext des Prozesses zerstört: opons-voxd verschwindet ganz aus `nvidia-smi` (0 MB VRAM). Das Neuladen dauert unter einer Sekunde, wenn die Modelldateien noch im Cache liegen, sonst einige Sekunden.

**Zustände des Tray-Symbols:**

| Symbol | Zustand |
|:---:|---|
| weißer Ring | Modelle geladen, bereit |
| rote Scheibe | Aufnahme |
| orange Scheibe | Transkription |
| grauer, durchgestrichener Ring (⊘) | Modelle entladen, GPU-Speicher frei, Diktat deaktiviert |
| blauer Dreiviertelbogen | Modelle werden neu geladen |

Solange die Modelle entladen sind, zeigt ein Klick auf das Symbol oder ein Push-to-talk-Kürzel eine Fehlermeldung an, statt aufzunehmen.

**Von Hand:** Rechtsklick auf das Tray-Symbol > *Unload models (free GPU memory)* (Modelle entladen), danach *Reload models* (Modelle neu laden).

**Über einen KI-Agenten (MCP):** den Daemon mit `-m` / `--mcp` starten (standardmäßig aus). `make` baut zusätzlich `opons-voxd-mcp`, einen [Model-Context-Protocol](https://modelcontextprotocol.io)-Server über stdio, der nur gegen die libc gelinkt ist (etwa 1,5 MB RAM). Er stellt drei Werkzeuge bereit: `unload_models`, `reload_models` und `get_status`. Mit Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

Die meisten anderen Clients (Claude Desktop, Cursor, LM Studio …) verwenden dieses JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Alle Details im [Haupt-README](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Lizenz

Alles unter MIT-Lizenz: Whisper-Modell (OpenAI), whisper.cpp (Georgi Gerganov) und dieses Projekt.

Vollständige Dokumentation: siehe [Haupt-README](../README.md).

---

## Codierungsstil

Dieses Projekt folgt dem **Linux-Kernel-Codierungsstil** (`Documentation/process/coding-style.rst`) mit Anpassungen von Olivier Pons: K&R-Klammern, 4 Leerzeichen Einrückung, `struct name` ohne Typedef, keine Präfixe (`s_`, `t_`, `e_`), `return value;` ohne Klammern, kernel-doc-Kommentare, maximal 80 Zeichen pro Zeile. Vollständige Details im [Haupt-README](../README.md).

## Push-to-talk-Kürzel

Das mitgelieferte `launch.sh` verwendet `ctrl+alt+w` (empfohlen für AZERTY) bzw. `ctrl+alt+z` (empfohlen für QWERTY). Die Taste liegt direkt neben `A`, sodass die ganze Kombination allein mit der linken Hand erreichbar ist, ohne die Schreibhaltung aufzugeben, und sie ist in den meisten Desktop-Umgebungen standardmäßig nicht belegt. Konfigurierbar über `OPONS_VOXD_PTT_HOTKEY`. Alle Details im [Haupt-README](../README.md#push-to-talk-hotkey).
