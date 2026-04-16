# Voice In Linux

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

1. Ein kleines Symbol erscheint im System-Tray
2. **Linksklick** → Aufnahme startet (Symbol wird rot)
3. **Erneuter Linksklick** → Aufnahme stoppt, Audio wird lokal via Whisper transkribiert
4. Der transkribierte Text wird automatisch in die Zwischenablage kopiert (beide X11-Selectionen)
5. Eine Desktop-Benachrichtigung zeigt den Text 10 Sekunden lang an
6. Einfügen mit `Ctrl+Shift+V`, `Shift+Insert` oder Mittelklick

---

## Funktionen

- **100% lokal** — Audio wird auf CPU oder GPU verarbeitet, nie versendet
- **GPU-beschleunigt** — NVIDIA CUDA-Unterstützung für nahezu sofortige Transkription
- **99 Sprachen** — unterstützt durch OpenAI Whisper
- **Leichtgewichtig** — einzelne C-Binärdatei (~100 KB), kein Python nötig
- **System-Tray-Integration** — unauffälliges Symbol in der Taskleiste
- **Doppelte Zwischenablage** — Text in PRIMARY und CLIPBOARD
- **Automatische Großschreibung** — Sätze werden automatisch kapitalisiert
- **Autostart** — beim Login konfigurierbar

---

## Installation

```bash
# 1. Systemabhängigkeiten
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin

# 2. Repository klonen
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux

# 3. whisper.cpp kompilieren + Modell herunterladen
make setup

# 4. Binärdatei kompilieren
make

# 5. Starten
./voice_in
```

---

## Lizenz

Alles unter MIT-Lizenz: Whisper-Modell (OpenAI), whisper.cpp (Georgi Gerganov) und dieses Projekt.

Vollständige Dokumentation: siehe [Haupt-README](../README.md).
