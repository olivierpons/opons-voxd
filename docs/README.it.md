# Voice In Linux

**Dettatura vocale 100% locale per Linux — la vostra voce non lascia mai il vostro computer.**

🌍 **Lingue:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md)

---

## Perché?

Volevo poter dettare testo in qualsiasi applicazione Linux senza inviare la mia voce a un server di terze parti. Tutte le soluzioni esistenti — estensioni del browser, API cloud, piattaforme SaaS — trasmettono il vostro audio a server remoti per la trascrizione. Ciò significa che ogni parola pronunciata transita attraverso Internet: email, documenti riservati, dati dei clienti, note personali — tutto.

Questo progetto è un'alternativa leggera e completamente locale. Funziona interamente sulla vostra macchina utilizzando [whisper.cpp](https://github.com/ggerganov/whisper.cpp), il modello di riconoscimento vocale open source di OpenAI. Non è necessaria alcuna connessione di rete dopo l'installazione iniziale. Nessun dato lascia mai il vostro computer.

Sviluppato con l'aiuto dell'intelligenza artificiale.

---

## Come funziona

1. Un'icona appare nel system tray
2. **Clic sinistro** → inizia la registrazione (l'icona diventa rossa)
3. **Clic sinistro di nuovo** → stop, l'audio viene trascritto localmente con Whisper
4. Il testo trascritto viene copiato automaticamente negli appunti (entrambe le selezioni X11)
5. Una notifica desktop mostra il testo per 10 secondi
6. Incollate con `Ctrl+Shift+V`, `Shift+Insert` o clic centrale

---


### Screenshots

| Inattivo | Registrazione |
|:---:|:---:|
| ![Inattivo](../screenshots/voice_in_inactive.png) | ![Registrazione](../screenshots/voice_in_active.png) |

## Caratteristiche

- **100% locale** — l'audio viene elaborato su CPU o GPU, mai inviato
- **Accelerazione GPU** — supporto NVIDIA CUDA per trascrizione quasi istantanea
- **99 lingue** — basato su OpenAI Whisper
- **Leggero** — singolo binario C (~100 KB), senza Python
- **Integrazione nel system tray** — icona discreta nella barra delle applicazioni
- **Doppi appunti** — testo copiato in PRIMARY e CLIPBOARD
- **Maiuscole automatiche** — le frasi vengono capitalizzate automaticamente
- **Avvio automatico** — configurabile all'accesso

---

## Installazione

```bash
# 1. Dipendenze di sistema
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin

# 2. Clonare il repository
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux

# 3. Compilare whisper.cpp e scaricare il modello
make setup

# 4. Compilare il binario
make

# 5. Avviare
./voice_in
```

---

## Licenza

Tutto sotto licenza MIT: modello Whisper (OpenAI), whisper.cpp (Georgi Gerganov) e questo progetto.

Documentazione completa: vedere il [README principale](../README.md).
