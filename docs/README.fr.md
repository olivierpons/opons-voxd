# opons-voxd

> Remarque : opons-voxd n’a aucun lien avec jakovius/voxd. Le nom a été choisi indépendamment : `opons` = O. Pons (l’auteur), `voxd` = vox + daemon (tradition latine et Unix).

> **⚠ Rétrocompatibilité rompue volontairement.** Ce projet s’appelait auparavant `voice_in_linux`. Le changement de nom a touché le nom du binaire, celui du fichier source, toutes les variables d’environnement (`VOICE_IN_*` → `OPONS_VOXD_*`) ainsi que la dépendance d’exécution à `xdotool` (remplacée par une synthèse XTest directe via `libxtst-dev`). Aucun alias, aucune période de transition : mettez à jour vos scripts de lancement, vos entrées de démarrage automatique et vos lignes `apt install`.

**Dictée vocale locale pour Linux : votre voix ne quitte jamais votre machine.**

🌍 **Traductions :**
[English](../README.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Mon workflow : dicter dans sa langue, taper en anglais](WORKFLOW.fr.md)** — un cas d’usage réel

---

## Pourquoi ?

Je voulais dicter du texte dans n’importe quelle application sous Linux sans envoyer ma voix à un serveur tiers. Toutes les solutions existantes que j’ai trouvées (extensions de navigateur, API cloud, plateformes SaaS) envoient votre audio à des serveurs distants pour le transcrire. Autrement dit, chaque mot prononcé transite par Internet : courriels, documents confidentiels, données clients, notes personnelles, absolument tout.

Ce projet est une alternative légère et entièrement locale. Il tourne intégralement sur votre machine grâce à [whisper.cpp](https://github.com/ggerganov/whisper.cpp) et à Whisper, le modèle open source de reconnaissance vocale d’OpenAI. Aucune connexion réseau n’est nécessaire après l’installation initiale. Aucune donnée ne quitte jamais votre ordinateur.

Développé avec l’aide de l’IA.

---

## Fonctionnement

Deux façons de dicter, utilisables en même temps.

### Mode icône — bascule et copie dans le presse-papiers

1. Une petite icône apparaît dans votre zone de notification
2. **Clic gauche** → l’enregistrement démarre (l’icône devient rouge)
3. **Nouveau clic gauche** → l’enregistrement s’arrête et l’audio est transcrit localement par Whisper
4. Le texte transcrit est automatiquement copié dans le presse-papiers (dans les deux sélections X11)
5. Une notification de bureau affiche le texte pendant 10 secondes
6. Collez-le où vous voulez avec `Ctrl+Shift+V`, `Shift+Insert` ou un clic milieu

Un **clic droit** sur l’icône ouvre un menu Toggle / Unload models / Reload models / Quit (marche/arrêt, déchargement des modèles, rechargement des modèles, quitter). Voir [Libérer le GPU pour l’IA locale](#libérer-le-gpu-pour-lia-locale-serveur-mcp).

### Mode push-to-talk — raccourci maintenu, saisie au curseur

1. **Maintenez `Ctrl+Shift+Space`** → l’enregistrement démarre (l’icône devient rouge)
2. **Relâchez les touches** → l’enregistrement s’arrête et l’audio est transcrit
3. Le texte transcrit est **saisi directement au curseur clavier** (via l’extension X11 XTest) dans la fenêtre qui a le focus

Le raccourci se configure avec `OPONS_VOXD_PTT_HOTKEY`. Un second raccourci indépendant (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `ctrl+alt+t` par défaut) fonctionne de la même façon, mais saisit une traduction anglaise à la place : voir le [README principal](../README.md#push-to-talk-and-translate-hotkey).

### Captures d’écran

| Au repos | Enregistrement |
|:---:|:---:|
| ![Icône au repos](../screenshots/opons_voxd_inactive.png) | ![Icône pendant l’enregistrement](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Un GPU NVIDIA avec CUDA est indispensable pour obtenir des temps de réponse acceptables.**
> Sans CUDA, la transcription prend **de 10 à 15 secondes** pour 15 secondes de parole (CPU seul).
> Avec CUDA, la même transcription prend **moins de 1 seconde**.

---

## Fonctionnalités

- **100 % local** — l’audio est traité par votre CPU ou votre GPU, sans jamais être envoyé ailleurs
- **Accélération GPU** — prise en charge de NVIDIA CUDA pour une transcription quasi instantanée
- **99 langues** — grâce à OpenAI Whisper
- **Léger** — du C pur, sans Python : le daemon, plus un serveur MCP optionnel de 25 Ko lié uniquement à la libc
- **Intégration à la zone de notification** — une icône discrète dans votre barre des tâches
- **Raccourci push-to-talk** — maintenez `Ctrl+Shift+Space` (configurable) pour enregistrer, relâchez pour que la transcription soit saisie au curseur clavier
- **Raccourci push-to-talk avec traduction** — un second raccourci, configurable indépendamment, saisit une traduction anglaise de vos paroles au lieu d’une transcription mot pour mot
- **Double presse-papiers** — le texte est envoyé dans les deux sélections X11, PRIMARY et CLIPBOARD
- **Notifications de bureau** — le texte transcrit s’affiche dans une notification
- **Commandes vocales** — un fichier de commandes par langue dans `commands/`, désactivées par défaut (activation : `OPONS_VOXD_COMMANDS=1`). Pour ajouter une langue, il suffit de créer `commands/xx.txt`, sans recompiler.
- **Notifications éphémères** — par défaut, les notifications apparaissent puis disparaissent complètement. `OPONS_VOXD_NOTIFY_PERSIST=1` les conserve dans l’historique.
- **Mode de notification** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` règle les notifications de réussite (par défaut `quiet` : bulle, sans son). Les erreurs s’affichent toujours avec un son, quel que soit le mode, sans exception. Le tableau complet figure dans le README principal.
- **Majuscules automatiques** — les phrases commencent automatiquement par une majuscule
- **Libération du GPU à la demande** — déchargez les modèles Whisper de la VRAM (depuis le menu de l’icône, ou par n’importe quel agent d’IA via le serveur MCP intégré), puis rechargez-les une fois votre tâche terminée
- **Démarrage automatique** — peut être configuré pour se lancer à l’ouverture de session

---

## Prérequis

### Système d’exploitation

| Élément | Minimum | Vérification |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Noyau Linux | 5.15+ | `uname -r` |
| Serveur d’affichage | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland n’est pas pris en charge (GtkStatusIcon nécessite X11). Linux Mint utilise X11 par défaut.

### Outils de compilation (obligatoires)

| Paquet | Minimum | Installation |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Bibliothèques de développement (obligatoires)

| Paquet | Minimum | Installation |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` n’est pas nécessaire : la compilation télécharge `portaudio.h` (via `curl`, au premier `make`) et se lie directement à `libportaudio.so.2`. Détails dans le [README principal](../README.md#development-libraries-required).

### Outils d’exécution (obligatoires)

| Outil | Rôle | Installation |
|---|---|---|
| xclip | Copie dans les presse-papiers X11 | `sudo apt install xclip` |
| notify-send | Notifications de bureau | `sudo apt install libnotify-bin` |

### GPU NVIDIA (facultatif mais déterminant pour les performances)

Sans CUDA, comptez **de 10 à 15 secondes** de traitement pour 15 secondes de parole. Avec CUDA, **moins de 1 seconde**. Le traitement est donc **de 10 à 50 fois** plus rapide.

| Composant | Minimum | Vérification |
|---|---|---|
| Pilote NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Si `nvidia-smi` fonctionne mais pas `nvcc`, installez le CUDA Toolkit :

```bash
sudo apt install nvidia-cuda-toolkit
```

Si la version de CUDA indiquée par `nvidia-smi` est **inférieure** à celle que signale `nvcc`, mettez à jour le pilote :

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Installation

```bash
# 1. Dépendances système
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Cloner le dépôt
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Compiler whisper.cpp et télécharger le modèle
make setup

# 4. Compiler les binaires : opons-voxd (le daemon) et opons-voxd-mcp (serveur MCP optionnel)
make

# 5. Lancer
./opons-voxd
```

---

## Libérer le GPU pour l’IA locale (serveur MCP)

Les modèles Whisper restent en permanence dans la mémoire du GPU (environ 3,8 Go pour `large-v3-turbo` + `medium`). Quand vous avez besoin de cette VRAM pour autre chose (un LLM local, un modèle d’images, un entraînement), opons-voxd peut les décharger puis les recharger, sans que vous ayez à le quitter. Le déchargement détruit aussi le contexte CUDA du processus : opons-voxd disparaît alors complètement de `nvidia-smi` (0 Mo de VRAM). Le rechargement prend moins d’une seconde si les fichiers des modèles sont encore dans le cache de pages, quelques secondes sinon.

**États de l’icône :**

| Icône | État |
|:---:|---|
| anneau blanc | modèles chargés, prêt |
| disque rouge | enregistrement en cours |
| disque orange | transcription en cours |
| anneau gris barré (⊘) | modèles déchargés, mémoire GPU libérée, dictée désactivée |
| arc bleu aux trois quarts | rechargement des modèles |

Tant que les modèles sont déchargés, un clic sur l’icône ou un appui sur un raccourci push-to-talk affiche une notification d’erreur au lieu de lancer l’enregistrement.

**À la main :** clic droit sur l’icône > *Unload models (free GPU memory)* (décharger les modèles), puis *Reload models* (recharger les modèles).

**Depuis un agent d’IA (MCP) :** lancez le daemon avec `-m` / `--mcp` (désactivé par défaut). `make` compile aussi `opons-voxd-mcp`, un serveur [Model Context Protocol](https://modelcontextprotocol.io) qui communique par stdio et n’est lié qu’à la libc (environ 1,5 Mo de RAM). Il expose trois outils : `unload_models`, `reload_models` et `get_status`. Avec Claude Code :

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

La plupart des autres clients (Claude Desktop, Cursor, LM Studio, etc.) utilisent ce JSON :

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Tous les détails dans le [README principal](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Raccourci push-to-talk

Le `launch.sh` fourni utilise `ctrl+alt+w` (recommandé en AZERTY) ou `ctrl+alt+z` (recommandé en QWERTY). Cette touche est voisine du `A` : toute la combinaison s’atteint de la seule main gauche (auriculaire sur `Ctrl`, pouce sur `Alt`, annulaire ou majeur sur `W`/`Z`) sans quitter votre position de frappe. De plus, la plupart des environnements de bureau ne l’attribuent pas par défaut. Le raccourci se configure avec `OPONS_VOXD_PTT_HOTKEY`. Tous les détails dans le [README principal](../README.md#push-to-talk-hotkey).

---

## Licence

Tout est sous licence MIT : le modèle Whisper (OpenAI), whisper.cpp (Georgi Gerganov) et ce projet.

Documentation complète : voir le [README principal](../README.md).

---

## Style de code

Ce projet suit le **style de codage du noyau Linux** (`Documentation/process/coding-style.rst`) avec des adaptations d’Olivier Pons : accolades K&R, indentation de 4 espaces, `struct name` sans typedef, pas de préfixes (`s_`, `t_`, `e_`), `return value;` sans parenthèses, commentaires kernel-doc, 80 caractères par ligne au maximum. Tous les détails dans le [README principal](../README.md).
