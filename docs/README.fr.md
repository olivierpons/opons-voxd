# Voice In Linux

**Dictation vocale 100% locale pour Linux — votre voix ne quitte jamais votre machine.**

🌍 **Langues :**
[English](../README.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md)

---

## Pourquoi ?

Je voulais pouvoir dicter du texte dans n'importe quelle application sous Linux sans envoyer ma voix à un serveur tiers. Toutes les solutions existantes — extensions de navigateur, API cloud, plateformes SaaS — transmettent votre audio à des serveurs distants pour la transcription. Cela signifie que chaque mot prononcé transite par Internet : emails, documents confidentiels, données clients, notes personnelles.

Ce projet est une alternative légère et entièrement locale. Il tourne exclusivement sur votre machine grâce à [whisper.cpp](https://github.com/ggerganov/whisper.cpp), le modèle de reconnaissance vocale open source d'OpenAI. Aucune connexion réseau n'est nécessaire après l'installation initiale. Aucune donnée ne quitte votre ordinateur.

Réalisé avec l'aide de l'intelligence artificielle.

---

## Fonctionnement

1. Une icône apparaît dans la barre des tâches (system tray)
2. **Clic gauche** → l'enregistrement démarre (l'icône devient rouge)
3. **Clic gauche** à nouveau → arrêt, transcription locale via Whisper
4. Le texte transcrit est copié dans le presse-papier (les deux sélections X11)
5. Une notification affiche le texte pendant 10 secondes
6. Collez avec `Ctrl+Shift+V`, `Shift+Insert` ou clic milieu

**Clic droit** sur l'icône pour un menu Toggle / Quitter.

### Captures d'écran

| Inactif | Enregistrement |
|:---:|:---:|
| ![Inactif](../screenshots/voice_in_inactive.png) | ![Enregistrement](../screenshots/voice_in_active.png) |

---

## Fonctionnalités

- **100% local** — l'audio est traité sur votre CPU ou GPU, jamais envoyé
- **Accélération GPU** — support NVIDIA CUDA pour une transcription quasi-instantanée
- **99 langues** — propulsé par OpenAI Whisper
- **Léger** — binaire C unique (~100 Ko), pas de Python, pas de dépendances runtime
- **Intégration system tray** — icône discrète dans la barre des tâches
- **Double presse-papier** — texte copié dans PRIMARY et CLIPBOARD
- **Notifications bureau** — le texte transcrit est affiché en notification
- **Commandes vocales** — commandes françaises intégrées ("point", "virgule", "nouvelle ligne"...), désactivées par défaut (`VOICE_IN_COMMANDS=1` pour activer)
- **Majuscules automatiques** — les phrases sont capitalisées automatiquement
- **Démarrage automatique** — configurable au login

---

## Prérequis

### Système d'exploitation

| Prérequis | Minimum | Vérification |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Noyau Linux | 5.15+ | `uname -r` |
| Serveur d'affichage | X11 | `echo $XDG_SESSION_TYPE` |

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
| libportaudio-dev | 19.6+ | `sudo apt install libportaudio-dev` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |

### Outils runtime (obligatoire)

| Outil | Rôle | Installation |
|---|---|---|
| xclip | Presse-papiers X11 | `sudo apt install xclip` |
| notify-send | Notifications bureau | `sudo apt install libnotify-bin` |

### GPU NVIDIA (optionnel, fortement recommandé)

| Composant | Minimum | Vérification |
|---|---|---|
| Driver NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

---

## Installation

```bash
# 1. Dépendances système
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin

# 2. Cloner le projet
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux

# 3. Compiler whisper.cpp + télécharger le modèle
make setup

# 4. Compiler le binaire
make

# 5. Lancer
./voice_in
```

---

## Licence

Tout est sous licence MIT : le modèle Whisper (OpenAI), whisper.cpp (Georgi Gerganov), et ce projet.

Documentation complète : voir le [README principal](../README.md).
