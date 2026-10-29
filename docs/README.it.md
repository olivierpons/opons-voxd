# opons-voxd

> Nota: opons-voxd non ha alcun legame con jakovius/voxd. Il nome è stato scelto in modo indipendente: `opons` = O. Pons (autore), `voxd` = vox + daemon (tradizione latina e Unix).

> **⚠ Retrocompatibilità interrotta di proposito.** Questo progetto si chiamava `voice_in_linux`. Il cambio di nome ha riguardato il nome del binario, quello del file sorgente, tutte le variabili d’ambiente (`VOICE_IN_*` → `OPONS_VOXD_*`) e la dipendenza a runtime da `xdotool` (sostituita dalla sintesi XTest diretta tramite `libxtst-dev`). Niente alias, nessun periodo di transizione: aggiorna i tuoi script di avvio, le voci di avvio automatico e le righe `apt install`.

**Dettatura vocale locale per Linux: la tua voce non lascia mai il tuo computer.**

🌍 **Traduzioni:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Il mio flusso di lavoro: dettare nella propria lingua, scrivere in inglese](WORKFLOW.it.md)** — un caso d’uso reale

---

## Perché?

Volevo dettare testo in qualsiasi applicazione su Linux senza inviare la mia voce a un server di terzi. Tutte le soluzioni esistenti che ho trovato (estensioni del browser, API cloud, piattaforme SaaS) inviano il tuo audio a server remoti per trascriverlo. Vuol dire che ogni parola pronunciata passa da internet: email, documenti riservati, dati dei clienti, note personali, proprio tutto.

Questo progetto è un’alternativa leggera e completamente locale. Funziona interamente sul tuo computer con [whisper.cpp](https://github.com/ggerganov/whisper.cpp) e Whisper, il modello open source di riconoscimento vocale di OpenAI. Dopo la configurazione iniziale non serve alcuna connessione di rete. Nessun dato esce mai dalla tua macchina.

Realizzato con l’aiuto dell’IA.

---

## Come funziona

Due modi per dettare, disponibili contemporaneamente.

### Modalità icona — attivazione e copia negli appunti

1. Nell’area di notifica compare una piccola icona
2. **Clic sinistro** → la registrazione parte (l’icona diventa rossa)
3. **Un altro clic sinistro** → la registrazione si ferma e l’audio viene trascritto localmente con Whisper
4. Il testo trascritto viene copiato automaticamente negli appunti (in entrambe le selezioni X11)
5. Una notifica sul desktop mostra il testo per 10 secondi
6. Incollalo dove vuoi con `Ctrl+Shift+V`, `Shift+Insert` o il clic centrale

Un **clic destro** sull’icona apre un menu con Toggle / Unload models / Reload models / Quit (attiva/disattiva, rimuovi i modelli dalla memoria, ricarica i modelli, esci). Vedi [Liberare la GPU per l’IA locale](#liberare-la-gpu-per-lia-locale-server-mcp).

### Modalità push-to-talk — scorciatoia tenuta premuta, digitazione al cursore

1. **Tieni premuto `Ctrl+Shift+Space`** → la registrazione parte (l’icona diventa rossa)
2. **Rilascia i tasti** → la registrazione si ferma e l’audio viene trascritto
3. Il testo trascritto viene **digitato direttamente al cursore della tastiera** (tramite l’estensione X11 XTest) nella finestra che ha il focus

La scorciatoia si configura con `OPONS_VOXD_PTT_HOTKEY`. Una seconda scorciatoia indipendente (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, predefinita `ctrl+alt+t`) funziona allo stesso modo, ma digita invece una traduzione in inglese: vedi il [README principale](../README.md#push-to-talk-and-translate-hotkey).

### Screenshot

| A riposo | In registrazione |
|:---:|:---:|
| ![Icona a riposo](../screenshots/opons_voxd_inactive.png) | ![Icona durante la registrazione](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Una GPU NVIDIA con CUDA è fondamentale per avere tempi di risposta accettabili.**
> Senza CUDA, la trascrizione richiede **da 10 a 15 secondi** per 15 secondi di parlato (solo CPU).
> Con CUDA, la stessa trascrizione richiede **meno di 1 secondo**.

---

## Funzionalità

- **100% locale** — l’audio viene elaborato dalla tua CPU o GPU e non viene mai inviato altrove
- **Accelerazione GPU** — supporto NVIDIA CUDA per una trascrizione quasi istantanea
- **99 lingue** — grazie a OpenAI Whisper
- **Leggero** — C puro, niente Python: il daemon, più un server MCP facoltativo da 25 KB collegato solo alla libc
- **Integrazione nell’area di notifica** — un’icona discreta nella barra delle applicazioni
- **Scorciatoia push-to-talk** — tieni premuto `Ctrl+Shift+Space` (configurabile) per registrare, rilascia per digitare la trascrizione al cursore della tastiera
- **Scorciatoia push-to-talk con traduzione** — una seconda scorciatoia, configurabile in modo indipendente, digita una traduzione in inglese di ciò che dici invece di una trascrizione letterale
- **Doppi appunti** — il testo viene inviato a entrambe le selezioni X11, PRIMARY e CLIPBOARD
- **Notifiche sul desktop** — il testo trascritto compare in una notifica
- **Comandi vocali** — un file di comandi per lingua in `commands/`, disattivati per impostazione predefinita (si attivano con `OPONS_VOXD_COMMANDS=1`). Per aggiungere una lingua basta creare `commands/xx.txt`, senza ricompilare.
- **Notifiche temporanee** — per impostazione predefinita le notifiche compaiono e poi scompaiono del tutto. `OPONS_VOXD_NOTIFY_PERSIST=1` le conserva nella cronologia.
- **Modalità di notifica** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` regola le notifiche di successo (predefinito `quiet`: fumetto, senza suono). Gli errori compaiono sempre con il suono, in ogni modalità e senza eccezioni. La tabella completa è nel README principale.
- **Maiuscole automatiche** — le frasi iniziano automaticamente con la maiuscola
- **GPU liberabile su richiesta** — rimuovi i modelli Whisper dalla VRAM (dal menu dell’icona o tramite qualsiasi agente IA con il server MCP integrato) e ricaricali quando hai finito
- **Avvio automatico** — può essere configurato per partire all’accesso

---

## Prerequisiti

### Sistema operativo

| Requisito | Minimo | Verifica |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Kernel Linux | 5.15+ | `uname -r` |
| Server grafico | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland non è supportato (GtkStatusIcon richiede X11). Linux Mint usa X11 per impostazione predefinita.

### Strumenti di compilazione (obbligatori)

| Pacchetto | Minimo | Installazione |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Librerie di sviluppo (obbligatorie)

| Pacchetto | Minimo | Installazione |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` non serve: la compilazione scarica `portaudio.h` (con `curl`, al primo `make`) e si collega direttamente a `libportaudio.so.2`. Dettagli nel [README principale](../README.md#development-libraries-required).

### Strumenti di esecuzione (obbligatori)

| Strumento | Scopo | Installazione |
|---|---|---|
| xclip | Copia negli appunti X11 | `sudo apt install xclip` |
| notify-send | Notifiche sul desktop | `sudo apt install libnotify-bin` |

### GPU NVIDIA (facoltativa ma decisiva per le prestazioni)

Senza CUDA, aspettati **da 10 a 15 secondi** di elaborazione per 15 secondi di parlato. Con CUDA, **meno di 1 secondo**. Cioè **da 10 a 50 volte** più veloce.

| Componente | Minimo | Verifica |
|---|---|---|
| Driver NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Se `nvidia-smi` funziona ma `nvcc` no, installa il CUDA Toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Se la versione di CUDA mostrata da `nvidia-smi` è **inferiore** a quella indicata da `nvcc`, aggiorna il driver:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Installazione

```bash
# 1. Dipendenze di sistema
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Clonare il repository
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Compilare whisper.cpp e scaricare il modello
make setup

# 4. Compilare i binari: opons-voxd (il daemon) e opons-voxd-mcp (server MCP facoltativo)
make

# 5. Avviare
./opons-voxd
```

---

## Liberare la GPU per l’IA locale (server MCP)

I modelli Whisper restano sempre nella memoria della GPU (circa 3,8 GB per `large-v3-turbo` + `medium`). Quando ti serve quella VRAM per altro (un LLM locale, un modello di immagini, un addestramento), opons-voxd può rimuoverli dalla memoria e ricaricarli in seguito, senza doverlo chiudere. La rimozione distrugge anche il contesto CUDA del processo, quindi opons-voxd sparisce del tutto da `nvidia-smi` (0 MB di VRAM). Il ricaricamento richiede meno di un secondo se i file dei modelli sono ancora nella cache delle pagine, qualche secondo altrimenti.

**Stati dell’icona:**

| Icona | Stato |
|:---:|---|
| anello bianco | modelli caricati, pronto |
| disco rosso | registrazione in corso |
| disco arancione | trascrizione in corso |
| anello grigio barrato (⊘) | modelli rimossi dalla memoria, memoria GPU liberata, dettatura disattivata |
| arco blu a tre quarti | ricaricamento dei modelli |

Finché i modelli non sono in memoria, un clic sull’icona o la pressione di una scorciatoia push-to-talk mostra una notifica di errore invece di avviare la registrazione.

**A mano:** clic destro sull’icona > *Unload models (free GPU memory)* (rimuovi i modelli dalla memoria), poi *Reload models* (ricarica i modelli).

**Da un agente IA (MCP):** avvia il daemon con `-m` / `--mcp` (disattivato per impostazione predefinita). `make` compila anche `opons-voxd-mcp`, un server [Model Context Protocol](https://modelcontextprotocol.io) su stdio, collegato solo alla libc (circa 1,5 MB di RAM). Espone tre strumenti: `unload_models`, `reload_models` e `get_status`. Con Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

La maggior parte degli altri client (Claude Desktop, Cursor, LM Studio…) usa questo JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Tutti i dettagli nel [README principale](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Scorciatoia push-to-talk

Il `launch.sh` incluso usa `ctrl+alt+w` (consigliata con AZERTY) o `ctrl+alt+z` (consigliata con QWERTY). Il tasto si trova accanto alla `A`, quindi l’intera combinazione si raggiunge con la sola mano sinistra (mignolo su `Ctrl`, pollice su `Alt`, anulare o medio su `W`/`Z`) senza perdere la posizione di scrittura. Inoltre, nella maggior parte degli ambienti desktop non è assegnata per impostazione predefinita. La scorciatoia si configura con `OPONS_VOXD_PTT_HOTKEY`. Tutti i dettagli nel [README principale](../README.md#push-to-talk-hotkey).

---

## Licenza

Tutto è sotto licenza MIT: il modello Whisper (OpenAI), whisper.cpp (Georgi Gerganov) e questo progetto.

Documentazione completa: vedi il [README principale](../README.md).

---

## Stile del codice

Questo progetto segue lo **stile di codifica del kernel Linux** (`Documentation/process/coding-style.rst`) con adattamenti di Olivier Pons: graffe K&R, indentazione di 4 spazi, `struct name` senza typedef, nessun prefisso (`s_`, `t_`, `e_`), `return value;` senza parentesi, commenti kernel-doc, al massimo 80 caratteri per riga. Tutti i dettagli nel [README principale](../README.md).
