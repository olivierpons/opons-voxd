# Mon workflow : dicter dans sa langue, taper en anglais

🌍 **Langues :**
[English](../WORKFLOW.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Je suis francophone de naissance, mais l’essentiel de mon travail de développement — messages de commit, commentaires dans le code, issues GitHub, discussions avec des équipes internationales, documentation — se fait en anglais. Écrire un anglais fluide n’est pas le plus dur ; le vrai goulot d’étranglement, c’est de *penser* en anglais pendant que le reste de votre cerveau tourne dans votre langue maternelle. La dictée le fait disparaître entièrement, à condition de ne pas avoir à vous traduire vous-même.

## Ma configuration

J’utilise opons-voxd avec deux raccourcis push-to-talk indépendants :

- **`Ctrl+Alt+W` — dictée normale.** Je maintiens le raccourci, je parle français, je relâche : le texte français est saisi au curseur. Pour les notes, les échanges avec des collègues francophones, tout ce qui doit rester en français.
- **`Ctrl+Alt+K` — dictée avec traduction.** Même geste, mais je parle français et c’est de l’*anglais* qui est saisi au curseur. Pour tout ce qui s’adresse à un public anglophone : messages de commit, descriptions de PR, commentaires dans le code, issues GitHub, canaux de discussion anglophones.

Pas de copier-coller dans un traducteur, pas de changement de contexte, rien à retaper. Je parle, et l’anglais apparaît là où se trouve déjà mon curseur.

## Pourquoi deux raccourcis et pas un seul

La traduction ne donne de bons résultats qu’avec un modèle réellement entraîné pour cette tâche. Pour le raccourci français du quotidien, j’utilise `large-v3-turbo` parce qu’il est rapide ; mais il traduit mal, car il a été optimisé pour la vitesse de transcription, pas pour la traduction. Le raccourci de traduction charge donc un second modèle (`medium`), qui, lui, traduit correctement. Les deux modèles résident en même temps dans la mémoire du GPU ; sur une carte qui dispose de plus de quelques Go de marge, ce surcoût est négligeable. Quand j’ai besoin de récupérer cette mémoire pour une tâche d’IA locale, l’agent d’IA qui l’exécute décharge les deux modèles via le serveur MCP d’opons-voxd, puis les recharge une fois son travail terminé ; pendant ce temps, l’icône de la zone de notification devient un anneau gris barré, ce qui m’indique que la dictée est désactivée (voir [Libérer le GPU pour l’IA locale](README.fr.md#libérer-le-gpu-pour-lia-locale-serveur-mcp)).

## À essayer

Tout se règle par la configuration normale d’opons-voxd : rien à forker ni à patcher. Pour `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` et `OPONS_VOXD_LANGUAGE`, voir les sections [Configuration](../README.md#configuration) et [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) du README principal (en anglais).

Si, au quotidien, vous parlez une autre langue que l’anglais mais travaillez en anglais, c’est sans doute la fonctionnalité la plus utile de ce projet.
