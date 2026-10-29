# opons-voxd

> Nota: opons-voxd non ha alcun legame con jakovius/voxd. Il nome è stato scelto in modo indipendente — `opons` = O. Pons (autore), `voxd` = vox + daemon (tradizione latina e Unix).

> **⚠ Compatibilità con le versioni precedenti interrotta di proposito.** Questo progetto si chiamava prima `voice_in_linux`. La ridenominazione ha toccato il nome del binario, il file sorgente, tutte le variabili d'ambiente (`VOICE_IN_*` → `OPONS_VOXD_*`) e la dipendenza a runtime da `xdotool` (sostituita dalla sintesi XTest diretta tramite `libxtst-dev`). Nessun alias né periodo di transizione: aggiornate gli script di avvio, le voci di avvio automatico e le righe `apt install`.

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

Due modi di dettare, disponibili contemporaneamente.

**Icona nel system tray: attiva/disattiva e copia negli appunti**

1. Un'icona appare nel system tray
2. **Clic sinistro** → inizia la registrazione (l'icona diventa rossa)
3. **Clic sinistro di nuovo** → stop, l'audio viene trascritto localmente con Whisper
4. Il testo trascritto viene copiato automaticamente negli appunti (entrambe le selezioni X11)
5. Una notifica desktop mostra il testo per 10 secondi
6. Incollate con `Ctrl+Shift+V`, `Shift+Insert` o clic centrale

Il **clic destro** apre un menu: attiva/disattiva, scarica o ricarica i modelli, esci.

**Push-to-talk: tenete premuta una scorciatoia, il testo compare al cursore**

1. **Tenete premuto `Ctrl+Shift+Space`** → inizia la registrazione
2. **Rilasciate i tasti** → stop, l'audio viene trascritto
3. Il testo viene **digitato direttamente al cursore** della finestra attiva (tramite l'estensione XTest di X11)

### Screenshot

| Inattivo | Registrazione |
|:---:|:---:|
| ![Inattivo](../screenshots/opons_voxd_inactive.png) | ![Registrazione](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Una GPU NVIDIA con CUDA è fondamentale per tempi di risposta accettabili.**
> Senza CUDA: **10–15 secondi** di elaborazione per 15 secondi di parlato.
> Con CUDA: **meno di 1 secondo**. Se avete una GPU NVIDIA (GTX 1060 o successiva), configurare CUDA deve essere la priorità assoluta.

## Caratteristiche

- **100% locale** — l'audio viene elaborato su CPU o GPU, mai inviato
- **Accelerazione GPU** — supporto NVIDIA CUDA per trascrizione quasi istantanea
- **99 lingue** — basato su OpenAI Whisper
- **Leggero** — C puro, senza Python: il daemon più un server MCP facoltativo da 25 KB, collegato solo alla libc
- **Integrazione nel system tray** — icona discreta nella barra delle applicazioni
- **Scorciatoia push-to-talk** — tenete premuto `Ctrl+Shift+Space` (configurabile) per registrare, rilasciate per digitare la trascrizione al cursore
- **Scorciatoia push-to-talk con traduzione** — una seconda scorciatoia, configurabile a parte, digita una traduzione in inglese di ciò che dite invece della trascrizione letterale (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, predefinita `ctrl+alt+t`)
- **Doppi appunti** — testo copiato in PRIMARY e CLIPBOARD
- **Notifiche desktop** — il testo trascritto appare in una notifica transitoria; `OPONS_VOXD_NOTIFY_PERSIST=1` per conservarla nella cronologia
- **Modalità di notifica** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` regola le notifiche di successo (predefinita `quiet`: fumetto senza suono). Gli errori vengono sempre mostrati con il suono, in ogni modalità e senza eccezioni. La tabella completa è nel README principale.
- **Comandi vocali** — file di comandi per lingua in `commands/` per punteggiatura e formattazione, disabilitati di default (`OPONS_VOXD_COMMANDS=1`). Per aggiungere una lingua, create `commands/xx.txt`, senza ricompilare.
- **Maiuscole automatiche** — le frasi iniziano automaticamente con la maiuscola
- **GPU liberata su richiesta** — scaricate i modelli Whisper dalla VRAM (dal menu del system tray o da qualsiasi agente IA tramite il server MCP integrato) e ricaricateli quando avete finito
- **Avvio automatico** — configurabile all'accesso

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

# 4. Compilare i due binari: opons-voxd (daemon) e opons-voxd-mcp (server MCP facoltativo)
make

# 5. Avviare
./opons-voxd
```

---

## Liberare la GPU per l'IA locale (server MCP)

I modelli Whisper restano sempre nella memoria della GPU. Quando vi serve quella VRAM per altro (un LLM locale, un modello di immagini, un addestramento), opons-voxd può scaricarli e ricaricarli in seguito, senza essere chiuso. Lo scaricamento distrugge anche il contesto CUDA del processo: opons-voxd sparisce da `nvidia-smi` (0 MB di VRAM). Il ricaricamento richiede meno di un secondo se i file del modello sono ancora in cache.

**Stati dell'icona nel system tray:**

| Icona | Stato |
|:---:|---|
| anello bianco | modelli caricati, pronto |
| disco rosso | registrazione |
| disco arancione | trascrizione |
| anello grigio barrato (⊘) | modelli scaricati, memoria GPU liberata, dettatura disattivata |
| arco blu a tre quarti | ricaricamento dei modelli |

Finché i modelli sono scaricati, un clic sull'icona o la pressione di una scorciatoia mostra una notifica di errore invece di registrare.

**A mano:** clic destro sull'icona → *Unload models (free GPU memory)* (scarica i modelli), poi *Reload models* (ricarica i modelli).

**Da un agente IA (MCP):** avviate il daemon con `-m` / `--mcp` (disattivato di default). `make` compila anche `opons-voxd-mcp`, un server [Model Context Protocol](https://modelcontextprotocol.io) su stdio, collegato solo alla libc (circa 1,5 MB di RAM), che espone tre strumenti: `unload_models`, `reload_models` e `get_status`. Con Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

La maggior parte degli altri client usa questo JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Dettagli nel [README principale](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Scorciatoia push-to-talk

Il `launch.sh` incluso usa `ctrl+alt+w` (consigliata su AZERTY) o `ctrl+alt+z` (consigliata su QWERTY). Il tasto si trova accanto alla `A`, quindi l'intera combinazione si raggiunge con la sola mano sinistra, e nella maggior parte degli ambienti desktop non è assegnata di default. Si configura con `OPONS_VOXD_PTT_HOTKEY`. Tutti i dettagli nel [README principale](../README.md#push-to-talk-hotkey).

---

## Licenza

Tutto sotto licenza MIT: modello Whisper (OpenAI), whisper.cpp (Georgi Gerganov) e questo progetto.

Documentazione completa: vedere il [README principale](../README.md).

---

## Stile di codifica

Questo progetto segue lo **stile di codifica del kernel Linux** (`Documentation/process/coding-style.rst`) con adattamenti di Olivier Pons: parentesi graffe K&R, indentazione a 4 spazi, `struct name` senza typedef, nessun prefisso artificiale (`s_`, `t_`, `e_`), `return value;` senza parentesi, commenti kernel-doc, massimo 80 caratteri per riga. Dettagli completi nel [README principale](../README.md).
