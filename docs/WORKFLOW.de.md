# Mein Workflow: in der eigenen Sprache diktieren, auf Englisch tippen

🌍 **Sprachen:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Meine Muttersprache ist Französisch, aber der Großteil meiner Arbeit als Entwickler – Commit-Nachrichten, Code-Kommentare, GitHub-Issues, Chats mit internationalen Teams, Dokumentation – läuft auf Englisch. Flüssiges Englisch zu tippen ist nicht das Problem; der eigentliche Engpass ist, auf Englisch zu *denken*, während der Rest Ihres Gehirns in Ihrer Muttersprache arbeitet. Diktieren beseitigt diesen Engpass vollständig, sofern Sie sich nicht selbst übersetzen müssen.

## Mein Setup

Ich nutze opons-voxd mit zwei unabhängigen Push-to-Talk-Tastenkürzeln:

- **`Ctrl+Alt+W` – normales Diktieren.** Gedrückt halten, Französisch sprechen, loslassen: Der französische Text wird am Cursor eingetippt. Für Notizen, Chats mit französischsprachigen Kollegen und alles, was auf Französisch bleiben soll.
- **`Ctrl+Alt+K` – Diktieren mit Übersetzung.** Derselbe Handgriff, aber ich spreche Französisch, und am Cursor erscheint *Englisch*. Für alles, was sich an ein englischsprachiges Publikum richtet: Commit-Nachrichten, PR-Beschreibungen, Code-Kommentare, GitHub-Issues, englischsprachige Chat-Kanäle.

Kein Kopieren in einen Übersetzer, kein Kontextwechsel, kein erneutes Abtippen. Ich spreche, und der englische Text erscheint genau dort, wo mein Cursor schon steht.

## Warum zwei Tastenkürzel statt einem

Gut übersetzen kann nur ein Modell, das tatsächlich für diese Aufgabe trainiert wurde. Für das alltägliche französische Tastenkürzel nutze ich `large-v3-turbo`, weil es schnell ist; es übersetzt allerdings schlecht, denn es wurde auf Transkriptionsgeschwindigkeit optimiert, nicht auf Übersetzung. Das Übersetzungs-Tastenkürzel lädt deshalb ein zweites Modell (`medium`), das ordentlich übersetzt. Beide Modelle liegen gleichzeitig im GPU-Speicher; bei einer Grafikkarte mit mehr als ein paar GB Reserve fällt der zusätzliche Speicherbedarf nicht ins Gewicht. Brauche ich diesen Speicher für eine lokale KI-Aufgabe, entlädt der KI-Agent, der sie ausführt, beide Modelle über den MCP-Server von opons-voxd und lädt sie danach wieder; währenddessen wird das Tray-Symbol zu einem grauen, durchgestrichenen Ring, sodass ich sehe, dass das Diktieren deaktiviert ist (siehe [GPU für lokale KI freigeben](README.de.md#gpu-für-lokale-ki-freigeben-mcp-server)).

## Ausprobieren

Das alles lässt sich mit der regulären Konfiguration von opons-voxd einstellen – ohne Fork, ohne Patch. Die Variablen `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` und `OPONS_VOXD_LANGUAGE` sind im englischen Haupt-README in den Abschnitten [Configuration](../README.md#configuration) und [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) beschrieben.

Wenn Sie im Alltag eine andere Sprache als Englisch sprechen, aber auf Englisch arbeiten, ist das wahrscheinlich die nützlichste Funktion dieses Projekts.
