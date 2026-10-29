# opons-voxd

> Remarque : opons-voxd n'a aucun lien avec jakovius/voxd. Le nom a été choisi indépendamment — `opons` = O. Pons (l'auteur), `voxd` = vox + daemon (tradition latine et Unix).

> **⚠ Rétrocompatibilité cassée volontairement.** Ce projet s'appelait précédemment `voice_in_linux`. Le renommage touche le binaire, le fichier source, toutes les variables d'environnement (`VOICE_IN_*` → `OPONS_VOXD_*`), et la dépendance runtime à `xdotool` (remplacée par une synthèse XTest directe via `libxtst-dev`). Aucun alias, aucune période de transition — mettez à jour vos scripts de lancement, vos entrées de démarrage automatique et vos lignes `apt install`.

**Dictée vocale 100 % locale pour Linux — votre voix ne quitte jamais votre machine.**

🌍 **Langues :**
[English](../README.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md)

---

## Pourquoi ?

Je voulais pouvoir dicter du texte dans n'importe quelle application sous Linux sans envoyer ma voix à un serveur tiers. Toutes les solutions existantes — extensions de navigateur, API cloud, plateformes SaaS — transmettent votre audio à des serveurs distants pour la transcription. Cela signifie que chaque mot prononcé transite par Internet : emails, documents confidentiels, données clients, notes personnelles.

Ce projet est une alternative légère et entièrement locale. Il tourne exclusivement sur votre machine grâce à [whisper.cpp](https://github.com/ggerganov/whisper.cpp), le modèle de reconnaissance vocale open source d'OpenAI. Aucune connexion réseau n'est nécessaire après l'installation initiale. Aucune donnée ne quitte votre ordinateur.

Réalisé avec l'aide de l'intelligence artificielle.

---

## Fonctionnement

Deux façons de dicter, utilisables en même temps.

### Icône de la barre des tâches — bascule, copie dans le presse-papier

1. Une icône apparaît dans la barre des tâches (system tray)
2. **Clic gauche** → l'enregistrement démarre (l'icône devient rouge)
3. **Clic gauche** à nouveau → arrêt, transcription locale via Whisper
4. Le texte transcrit est copié dans le presse-papier (les deux sélections X11)
5. Une notification affiche le texte pendant 10 secondes
6. Collez avec `Ctrl+Shift+V`, `Shift+Insert` ou clic milieu

**Clic droit** sur l'icône pour un menu Toggle / Unload models / Reload models / Quit (voir [Libérer le GPU](#libérer-le-gpu-pour-lia-locale-serveur-mcp)).

### Push-to-talk — maintenir un raccourci, le texte s'écrit au curseur

1. **Maintenez `Ctrl+Shift+Space`** → l'enregistrement démarre (l'icône devient rouge)
2. **Relâchez les touches** → arrêt et transcription
3. Le texte est **tapé directement au curseur** (via l'extension X11 XTest), dans la fenêtre qui a le focus

Le raccourci se règle avec `OPONS_VOXD_PTT_HOTKEY`. Un second raccourci indépendant (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `ctrl+alt+t` par défaut) fonctionne de la même façon mais tape une traduction en anglais — voir le [README principal](../README.md#push-to-talk-and-translate-hotkey).

### Captures d'écran

| Inactif | Enregistrement |
|:---:|:---:|
| ![Inactif](../screenshots/opons_voxd_inactive.png) | ![Enregistrement](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Un GPU NVIDIA avec CUDA est indispensable pour des temps de réponse confortables.**
> Sans CUDA, comptez **10 à 15 secondes** pour 15 secondes de parole (CPU seul).
> Avec CUDA, la même transcription prend **moins d'1 seconde**.

---

## Fonctionnalités

- **100 % local** — l'audio est traité sur votre CPU ou GPU, jamais envoyé
- **Accélération GPU** — support NVIDIA CUDA pour une transcription quasi instantanée
- **99 langues** — propulsé par OpenAI Whisper
- **Léger** — du C pur, pas de Python : le daemon, plus un serveur MCP optionnel de 25 Ko lié uniquement à la libc
- **Intégration system tray** — icône discrète dans la barre des tâches
- **Raccourci push-to-talk** — maintenez `Ctrl+Shift+Space` (configurable) pour enregistrer, relâchez pour que le texte s'écrive au curseur
- **Raccourci push-to-talk avec traduction** — un second raccourci, réglable séparément, tape une traduction anglaise de ce que vous dites au lieu de la transcription mot pour mot
- **Double presse-papier** — texte copié dans PRIMARY et CLIPBOARD
- **Notifications bureau** — le texte transcrit est affiché en notification
- **Commandes vocales** — fichiers de commandes par langue dans `commands/`, désactivées par défaut (`OPONS_VOXD_COMMANDS=1`). Ajoutez une langue en créant `commands/xx.txt`, sans recompiler.
- **Notifications transitoires** — par défaut les notifications apparaissent puis disparaissent complètement. `OPONS_VOXD_NOTIFY_PERSIST=1` pour les garder dans l'historique.
- **Mode de notification** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` contrôle les notifications de succès (défaut `quiet` : bulle sans son). Les erreurs s'affichent et sonnent toujours, dans tous les modes, sans exception. Voir le README principal pour le tableau complet.
- **Majuscules automatiques** — chaque phrase commence automatiquement par une majuscule
- **Libérer le GPU à la demande** — déchargez les modèles Whisper de la VRAM (menu de l'icône, ou n'importe quel agent IA via le serveur MCP intégré), puis rechargez-les une fois terminé
- **Démarrage automatique** — configurable au login

---

## Prérequis

### Système d'exploitation

| Prérequis | Minimum | Vérification |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Noyau Linux | 5.15+ | `uname -r` |
| Serveur d'affichage | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland n'est pas pris en charge (GtkStatusIcon exige X11). Linux Mint utilise X11 par défaut.

### Compilation (obligatoire)

| Paquet | Minimum | Installation |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Bibliothèques de développement (obligatoire)

| Paquet | Minimum | Installation |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` n'est pas nécessaire : le build récupère `portaudio.h` (via `curl`, au premier `make`) et se lie directement à `libportaudio.so.2`. Détails dans le [README principal](../README.md#development-libraries-required).

### Outils runtime (obligatoire)

| Outil | Rôle | Installation |
|---|---|---|
| xclip | Presse-papiers X11 | `sudo apt install xclip` |
| notify-send | Notifications bureau | `sudo apt install libnotify-bin` |

### GPU NVIDIA (optionnel mais critique pour les performances)

Sans CUDA, comptez **10 à 15 secondes** de traitement pour 15 secondes de parole. Avec CUDA, comptez **moins d'1 seconde**. C'est un facteur **10 à 50x**.

| Composant | Minimum | Vérification |
|---|---|---|
| Driver NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Si `nvidia-smi` fonctionne mais pas `nvcc`, installez le toolkit CUDA :

```bash
sudo apt install nvidia-cuda-toolkit
```

Si la version CUDA affichée par `nvidia-smi` est **inférieure** à celle de `nvcc`, mettez à jour le driver :

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

# 2. Cloner le projet
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Compiler whisper.cpp + télécharger le modèle
make setup

# 4. Compiler les binaires : opons-voxd (le daemon) et opons-voxd-mcp (serveur MCP optionnel)
make

# 5. Lancer
./opons-voxd
```

---

## Libérer le GPU pour l'IA locale (serveur MCP)

Les modèles Whisper occupent la mémoire GPU en permanence (environ 3,8 Go pour `large-v3-turbo` + `medium`). Quand vous avez besoin de cette VRAM pour autre chose — un LLM local, un modèle d'image, un entraînement — opons-voxd peut les décharger puis les recharger plus tard, sans qu'il faille le quitter. Le déchargement détruit aussi le contexte CUDA du processus : opons-voxd disparaît de `nvidia-smi` (0 Mo de VRAM). Le rechargement prend moins d'une seconde si les fichiers des modèles sont encore en cache, quelques secondes sinon.

**États de l'icône :**

| Icône | État |
|:---:|---|
| anneau blanc | modèles chargés, prêt |
| disque rouge | enregistrement |
| disque orange | transcription |
| anneau gris barré (⊘) | modèles déchargés, mémoire GPU libérée, dictée désactivée |
| arc bleu aux trois quarts | rechargement des modèles |

Tant que les modèles sont déchargés, un clic sur l'icône ou un raccourci push-to-talk affiche une notification d'erreur au lieu d'enregistrer.

**À la main :** clic droit sur l'icône > *Unload models (free GPU memory)* (décharger les modèles), puis *Reload models* (recharger).

**Depuis un agent IA (MCP) :** lancez le daemon avec `-m` / `--mcp` (désactivé par défaut). `make` compile aussi `opons-voxd-mcp`, un serveur [Model Context Protocol](https://modelcontextprotocol.io) en stdio, lié uniquement à la libc (environ 1,5 Mo de RAM). Il expose trois outils : `unload_models`, `reload_models` et `get_status`. Avec Claude Code :

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

La plupart des autres clients (Claude Desktop, Cursor, LM Studio…) utilisent ce JSON :

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Détails complets dans le [README principal](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Licence

Tout est sous licence MIT : le modèle Whisper (OpenAI), whisper.cpp (Georgi Gerganov), et ce projet.

Documentation complète : voir le [README principal](../README.md).

---

## Style de code

Ce projet suit le **style du noyau Linux** (`Documentation/process/coding-style.rst`) avec des adaptations par Olivier Pons : accolades K&R, indentation 4 espaces, `struct name` sans typedef, pas de préfixes (`s_`, `t_`, `e_`), `return value;` sans parenthèses, commentaires kernel-doc, 80 caractères par ligne maximum. Détails complets dans le [README principal](../README.md).

## Raccourci push-to-talk

Le `launch.sh` fourni utilise `ctrl+alt+w` (recommandé sur AZERTY) ou `ctrl+alt+z` (recommandé sur QWERTY). La touche est juste à côté de `A`, donc tout le combo est atteignable avec la seule main gauche (auriculaire sur `Ctrl`, pouce sur `Alt`, annulaire/majeur sur `W`/`Z`) sans casser la posture de frappe, et n'est pris par aucun raccourci système par défaut sur la plupart des environnements de bureau. À configurer via `OPONS_VOXD_PTT_HOTKEY`. Détails complets dans le [README principal](../README.md#push-to-talk-hotkey).
