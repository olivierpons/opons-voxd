# Il mio flusso di lavoro: dettare nella propria lingua, scrivere in inglese

🌍 **Lingue:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Sono madrelingua francese, ma quasi tutto il mio lavoro di sviluppo — messaggi di commit, commenti nel codice, issue su GitHub, chat con team internazionali, documentazione — si svolge in inglese. Scrivere un inglese scorrevole non è la parte difficile; il vero collo di bottiglia è *pensare* in inglese mentre il resto del cervello continua a ragionare nella tua lingua madre. La dettatura elimina del tutto questo collo di bottiglia, a patto di non doverti tradurre da solo.

## La mia configurazione

Uso opons-voxd con due scorciatoie push-to-talk indipendenti:

- **`Ctrl+Alt+W` — dettatura normale.** La tengo premuta, parlo in francese, la rilascio: il testo francese viene digitato al cursore. Per appunti, chat con colleghi francofoni e tutto ciò che deve restare in francese.
- **`Ctrl+Alt+K` — dettatura con traduzione.** Stesso gesto, ma parlo in francese e al cursore compare l’*inglese*. Per tutto ciò che è destinato a un pubblico anglofono: messaggi di commit, descrizioni di PR, commenti nel codice, issue su GitHub, canali chat in inglese.

Niente copia e incolla in un traduttore, niente cambi di contesto, niente da riscrivere. Parlo, e l’inglese compare proprio dove si trova già il cursore.

## Perché due scorciatoie e non una

Una traduzione riesce bene solo con un modello addestrato davvero per quel compito. Per la scorciatoia quotidiana in francese uso `large-v3-turbo` perché è veloce, ma traduce male: è stato ottimizzato per la velocità di trascrizione, non per la traduzione. Per questo la scorciatoia di traduzione carica un secondo modello (`medium`), che invece traduce come si deve. I due modelli stanno contemporaneamente nella memoria della GPU; su una scheda con più di qualche GB di margine, il consumo aggiuntivo è trascurabile. Quando mi serve recuperare quella memoria per un lavoro di IA locale, l’agente IA che lo esegue rimuove entrambi i modelli dalla memoria tramite il server MCP di opons-voxd e poi li ricarica; nel frattempo l’icona nell’area di notifica diventa un anello grigio barrato, così so che la dettatura è disattivata (vedi [Liberare la GPU per l’IA locale](README.it.md#liberare-la-gpu-per-lia-locale-server-mcp)).

## Provalo

Tutto si imposta con la normale configurazione di opons-voxd: nessun fork, nessuna patch. Le variabili `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` e `OPONS_VOXD_LANGUAGE` sono descritte nelle sezioni [Configuration](../README.md#configuration) e [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) del README principale, in inglese.

Se ogni giorno parli una lingua diversa dall’inglese ma lavori in inglese, questa è probabilmente la funzionalità più utile del progetto.
