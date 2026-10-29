# opons-voxd

> Nota: opons-voxd no tiene ninguna relación con jakovius/voxd. El nombre se eligió de forma independiente: `opons` = O. Pons (autor), `voxd` = vox + daemon (tradición latina y Unix).

> **⚠ Compatibilidad con versiones anteriores rota a propósito.** Este proyecto se llamaba antes `voice_in_linux`. El cambio de nombre afectó al nombre del binario, al del archivo fuente, a todas las variables de entorno (`VOICE_IN_*` → `OPONS_VOXD_*`) y a la dependencia en tiempo de ejecución de `xdotool` (sustituida por la síntesis XTest directa mediante `libxtst-dev`). Sin alias ni periodo de transición: actualiza tus scripts de arranque, tus entradas de inicio automático y tus líneas `apt install`.

**Dictado por voz local para Linux: tu voz nunca sale de tu equipo.**

🌍 **Traducciones:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Mi flujo de trabajo: dictar en tu idioma, escribir en inglés](WORKFLOW.es.md)**, un caso de uso real

---

## ¿Por qué?

Quería dictar texto en cualquier aplicación de Linux sin enviar mi voz a un servidor de terceros. Todas las soluciones que encontré (extensiones de navegador, API en la nube, plataformas SaaS) mandan tu audio a servidores remotos para transcribirlo. Es decir, cada palabra que pronuncias pasa por internet: correos, documentos confidenciales, datos de clientes, notas personales; absolutamente todo.

Este proyecto es una alternativa ligera y totalmente local. Se ejecuta por completo en tu equipo con [whisper.cpp](https://github.com/ggerganov/whisper.cpp) y Whisper, el modelo de reconocimiento de voz de código abierto de OpenAI. Tras la instalación inicial no hace falta conexión a la red. Ningún dato sale nunca de tu equipo.

Desarrollado con ayuda de la IA.

---

## Cómo funciona

Dos formas de dictar, disponibles a la vez.

### Modo icono de la bandeja: activación y copia al portapapeles

1. Aparece un pequeño icono en la bandeja del sistema
2. **Clic izquierdo** → empieza la grabación (el icono se vuelve rojo)
3. **Otro clic izquierdo** → se detiene la grabación y el audio se transcribe localmente con Whisper
4. El texto transcrito se copia automáticamente al portapapeles (en las dos selecciones de X11)
5. Una notificación de escritorio muestra el texto durante 10 segundos
6. Pégalo donde quieras con `Ctrl+Shift+V`, `Shift+Insert` o el clic central

Un **clic derecho** en el icono abre un menú con Toggle / Unload models / Reload models / Quit (activar o desactivar, descargar los modelos de la memoria, volver a cargarlos, salir). Consulta [Liberar la GPU para la IA local](#liberar-la-gpu-para-la-ia-local-servidor-mcp).

### Modo push-to-talk: atajo mantenido y escritura en el cursor

1. **Mantén pulsado `Ctrl+Shift+Space`** → empieza la grabación (el icono se vuelve rojo)
2. **Suelta las teclas** → se detiene la grabación y el audio se transcribe
3. El texto transcrito se **escribe directamente en el cursor del teclado** (mediante la extensión XTest de X11) en la ventana que tenga el foco

El atajo se configura con `OPONS_VOXD_PTT_HOTKEY`. Un segundo atajo independiente (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `ctrl+alt+t` por defecto) funciona igual, pero escribe en su lugar una traducción al inglés; consulta el [README principal](../README.md#push-to-talk-and-translate-hotkey).

### Capturas de pantalla

| En reposo | Grabando |
|:---:|:---:|
| ![Icono en reposo](../screenshots/opons_voxd_inactive.png) | ![Icono durante la grabación](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Una GPU NVIDIA con CUDA es imprescindible para tener tiempos de respuesta aceptables.**
> Sin CUDA, la transcripción tarda **de 10 a 15 segundos** por cada 15 segundos de voz (solo CPU).
> Con CUDA, la misma transcripción tarda **menos de 1 segundo**.

---

## Características

- **100 % local**: el audio se procesa en tu CPU o GPU y nunca se envía a ningún sitio
- **Aceleración por GPU**: compatibilidad con NVIDIA CUDA para una transcripción casi instantánea
- **99 idiomas**: gracias a OpenAI Whisper
- **Ligero**: C puro, sin Python; el daemon y, opcionalmente, un servidor MCP de 25 KB enlazado solo con la libc
- **Integración en la bandeja del sistema**: un icono discreto en la barra de tareas
- **Atajo push-to-talk**: mantén pulsado `Ctrl+Shift+Space` (configurable) para grabar y suéltalo para que la transcripción se escriba en el cursor del teclado
- **Atajo push-to-talk con traducción**: un segundo atajo, configurable por separado, escribe una traducción al inglés de lo que dices en lugar de una transcripción literal
- **Doble portapapeles**: el texto se envía a las dos selecciones de X11, PRIMARY y CLIPBOARD
- **Notificaciones de escritorio**: el texto transcrito aparece en una notificación
- **Comandos de voz**: un archivo de comandos por idioma en `commands/`, desactivados por defecto (se activan con `OPONS_VOXD_COMMANDS=1`). Para añadir un idioma basta con crear `commands/xx.txt`, sin recompilar.
- **Notificaciones efímeras**: por defecto, las notificaciones aparecen y luego desaparecen por completo. `OPONS_VOXD_NOTIFY_PERSIST=1` las conserva en el historial.
- **Modo de notificación**: `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` controla las notificaciones de éxito (por defecto `quiet`: burbuja sin sonido). Los errores se muestran siempre con sonido, en todos los modos y sin excepción. La tabla completa está en el README principal.
- **Mayúsculas automáticas**: las frases empiezan automáticamente con mayúscula
- **GPU liberable cuando quieras**: descarga los modelos Whisper de la VRAM (desde el menú de la bandeja o mediante cualquier agente de IA a través del servidor MCP integrado) y vuelve a cargarlos al terminar
- **Inicio automático**: puede configurarse para arrancar al iniciar sesión

---

## Requisitos previos

### Sistema operativo

| Requisito | Mínimo | Comprobación |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Kernel de Linux | 5.15+ | `uname -r` |
| Servidor gráfico | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland no es compatible (GtkStatusIcon necesita X11). Linux Mint usa X11 por defecto.

### Herramientas de compilación (obligatorias)

| Paquete | Mínimo | Instalación |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Bibliotecas de desarrollo (obligatorias)

| Paquete | Mínimo | Instalación |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> No hace falta `libportaudio-dev`: la compilación obtiene `portaudio.h` (con `curl`, en el primer `make`) y enlaza directamente con `libportaudio.so.2`. Más detalles en el [README principal](../README.md#development-libraries-required).

### Herramientas de ejecución (obligatorias)

| Herramienta | Función | Instalación |
|---|---|---|
| xclip | Copiar a los portapapeles de X11 | `sudo apt install xclip` |
| notify-send | Notificaciones de escritorio | `sudo apt install libnotify-bin` |

### GPU NVIDIA (opcional, pero decisiva para el rendimiento)

Sin CUDA, cuenta con **entre 10 y 15 segundos** de procesamiento por cada 15 segundos de voz. Con CUDA, **menos de 1 segundo**. Es decir, **de 10 a 50 veces** más rápido.

| Componente | Mínimo | Comprobación |
|---|---|---|
| Controlador NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Si `nvidia-smi` funciona pero `nvcc` no, instala el CUDA Toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Si la versión de CUDA que muestra `nvidia-smi` es **inferior** a la que indica `nvcc`, actualiza el controlador:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Instalación

```bash
# 1. Dependencias del sistema
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Clonar el repositorio
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Compilar whisper.cpp y descargar el modelo
make setup

# 4. Compilar los binarios: opons-voxd (el daemon) y opons-voxd-mcp (servidor MCP opcional)
make

# 5. Ejecutar
./opons-voxd
```

---

## Liberar la GPU para la IA local (servidor MCP)

Los modelos Whisper permanecen todo el tiempo en la memoria de la GPU (unos 3,8 GB para `large-v3-turbo` + `medium`). Cuando necesitas esa VRAM para otra cosa (un LLM local, un modelo de imágenes, un entrenamiento), opons-voxd puede descargarlos de la memoria y volver a cargarlos después, sin que tengas que cerrarlo. Al descargarlos también se destruye el contexto CUDA del proceso, así que opons-voxd desaparece por completo de `nvidia-smi` (0 MB de VRAM). Volver a cargarlos tarda menos de un segundo si los archivos de los modelos siguen en la caché de páginas, y unos segundos en caso contrario.

**Estados del icono de la bandeja:**

| Icono | Estado |
|:---:|---|
| anillo blanco | modelos cargados, listo |
| disco rojo | grabando |
| disco naranja | transcribiendo |
| anillo gris tachado (⊘) | modelos descargados, memoria de la GPU liberada, dictado desactivado |
| arco azul de tres cuartos | volviendo a cargar los modelos |

Mientras los modelos están descargados, hacer clic en el icono o pulsar un atajo push-to-talk muestra una notificación de error en lugar de empezar a grabar.

**A mano:** clic derecho en el icono de la bandeja > *Unload models (free GPU memory)* (descargar los modelos de la memoria) y después *Reload models* (volver a cargar los modelos).

**Desde un agente de IA (MCP):** inicia el daemon con `-m` / `--mcp` (desactivado por defecto). `make` también compila `opons-voxd-mcp`, un servidor [Model Context Protocol](https://modelcontextprotocol.io) sobre stdio, enlazado solo con la libc (unos 1,5 MB de RAM). Ofrece tres herramientas: `unload_models`, `reload_models` y `get_status`. Con Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

La mayoría de los demás clientes (Claude Desktop, Cursor, LM Studio…) usan este JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Todos los detalles en el [README principal](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Atajo push-to-talk

El `launch.sh` incluido usa `ctrl+alt+w` (recomendado en AZERTY) o `ctrl+alt+z` (recomendado en QWERTY). La tecla está junto a la `A`, así que toda la combinación se alcanza solo con la mano izquierda (meñique en `Ctrl`, pulgar en `Alt`, anular o medio en `W`/`Z`) sin perder la postura de escritura. Además, la mayoría de los entornos de escritorio no la tienen asignada por defecto. El atajo se configura con `OPONS_VOXD_PTT_HOTKEY`. Todos los detalles en el [README principal](../README.md#push-to-talk-hotkey).

---

## Licencia

Todo está bajo licencia MIT: el modelo Whisper (OpenAI), whisper.cpp (Georgi Gerganov) y este proyecto.

Documentación completa: consulta el [README principal](../README.md).

---

## Estilo de código

Este proyecto sigue el **estilo de código del kernel de Linux** (`Documentation/process/coding-style.rst`) con adaptaciones de Olivier Pons: llaves K&R, sangría de 4 espacios, `struct name` sin typedef, sin prefijos (`s_`, `t_`, `e_`), `return value;` sin paréntesis, comentarios kernel-doc, 80 caracteres por línea como máximo. Todos los detalles en el [README principal](../README.md).
