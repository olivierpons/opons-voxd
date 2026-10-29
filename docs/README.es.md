# opons-voxd

> Nota: opons-voxd no tiene ninguna relación con jakovius/voxd. El nombre se eligió de forma independiente — `opons` = O. Pons (autor), `voxd` = vox + daemon (tradición latina y Unix).

> **⚠ Compatibilidad con versiones anteriores rota a propósito.** Este proyecto se llamaba antes `voice_in_linux`. El cambio de nombre afectó al binario, al archivo fuente, a todas las variables de entorno (`VOICE_IN_*` → `OPONS_VOXD_*`) y a la dependencia de ejecución de `xdotool` (sustituida por la síntesis directa de XTest mediante `libxtst-dev`). No hay alias ni periodo de transición: actualice sus scripts de arranque, sus entradas de inicio automático y sus líneas `apt install`.

**Dictado de voz a texto 100% local para Linux — su voz nunca sale de su máquina.**

🌍 **Idiomas:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Italiano](README.it.md)

---

## ¿Por qué?

Quería poder dictar texto en cualquier aplicación de Linux sin enviar mi voz a un servidor externo. Todas las soluciones existentes — extensiones de navegador, APIs en la nube, plataformas SaaS — transmiten su audio a servidores remotos para la transcripción. Esto significa que cada palabra que pronuncia transita por Internet: correos electrónicos, documentos confidenciales, datos de clientes, notas personales — todo.

Este proyecto es una alternativa ligera y completamente local. Se ejecuta íntegramente en su máquina utilizando [whisper.cpp](https://github.com/ggerganov/whisper.cpp), el modelo de reconocimiento de voz de código abierto de OpenAI. No se necesita conexión a Internet después de la instalación inicial. Ningún dato sale de su ordenador.

Desarrollado con la ayuda de inteligencia artificial.

---

## Funcionamiento

Dos formas de dictar, disponibles a la vez.

**Icono de la bandeja: alternar y copiar al portapapeles**

1. Un icono aparece en la bandeja del sistema
2. **Clic izquierdo** → comienza la grabación (el icono se vuelve rojo)
3. **Clic izquierdo de nuevo** → se detiene, el audio se transcribe localmente con Whisper
4. El texto transcrito se copia automáticamente al portapapeles (ambas selecciones X11)
5. Una notificación muestra el texto durante 10 segundos
6. Pegue con `Ctrl+Shift+V`, `Shift+Insert` o clic central

Con el **clic derecho** se abre un menú: alternar, descargar o recargar los modelos, salir.

**Pulsar para hablar: mantener un atajo y escribir en el cursor**

1. **Mantenga pulsado `Ctrl+Shift+Space`** → comienza la grabación
2. **Suelte las teclas** → se detiene y el audio se transcribe
3. El texto se **escribe directamente en el cursor** de la ventana activa (mediante la extensión XTest de X11)

### Capturas de pantalla

| Inactivo | Grabando |
|:---:|:---:|
| ![Inactivo](../screenshots/opons_voxd_inactive.png) | ![Grabando](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Una GPU NVIDIA con CUDA es fundamental para tener tiempos de respuesta aceptables.**
> Sin CUDA: **10–15 segundos** de procesamiento por cada 15 segundos de habla.
> Con CUDA: **menos de 1 segundo**. Si tiene una GPU NVIDIA (GTX 1060 o posterior), configurar CUDA debe ser su máxima prioridad.

## Características

- **100% local** — el audio se procesa en CPU o GPU, nunca se envía
- **Aceleración GPU** — soporte NVIDIA CUDA para transcripción casi instantánea
- **99 idiomas** — impulsado por OpenAI Whisper
- **Ligero** — C puro, sin Python: el daemon y, opcionalmente, un servidor MCP de 25 KB enlazado solo con la libc
- **Integración en la bandeja** — icono discreto en la barra de tareas
- **Atajo de pulsar para hablar** — mantenga `Ctrl+Shift+Space` (configurable) para grabar y suelte para escribir la transcripción en el cursor
- **Atajo de pulsar para hablar y traducir** — un segundo atajo, configurable por separado, escribe una traducción al inglés de lo que dice en lugar de la transcripción literal (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, por defecto `ctrl+alt+t`)
- **Doble portapapeles** — texto copiado en PRIMARY y CLIPBOARD
- **Notificaciones de escritorio** — el texto transcrito aparece en una notificación transitoria; `OPONS_VOXD_NOTIFY_PERSIST=1` para conservarla en el historial
- **Modo de notificación** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` controla las notificaciones de éxito (por defecto `quiet`: burbuja sin sonido). Los errores se muestran siempre con sonido, en todos los modos y sin excepción. La tabla completa está en el README principal.
- **Comandos de voz** — archivos de comandos por idioma en `commands/` para la puntuación y el formato, desactivados por defecto (`OPONS_VOXD_COMMANDS=1`). Para añadir un idioma, cree `commands/xx.txt`, sin recompilar.
- **Mayúsculas automáticas** — las oraciones se capitalizan automáticamente
- **Liberar la GPU bajo demanda** — descargue los modelos Whisper de la VRAM (desde el menú de la bandeja o desde cualquier agente de IA a través del servidor MCP integrado) y vuelva a cargarlos al terminar
- **Inicio automático** — configurable al iniciar sesión

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

# 4. Compilar los dos binarios: opons-voxd (daemon) y opons-voxd-mcp (servidor MCP opcional)
make

# 5. Ejecutar
./opons-voxd
```

---

## Liberar la GPU para la IA local (servidor MCP)

Los modelos Whisper ocupan la memoria de la GPU de forma permanente. Cuando necesite esa VRAM para otra cosa (un LLM local, un modelo de imágenes, un entrenamiento), opons-voxd puede descargarlos y volver a cargarlos después sin cerrarse. La descarga destruye también el contexto CUDA del proceso: opons-voxd desaparece de `nvidia-smi` (0 MB de VRAM). La recarga tarda menos de un segundo si los archivos del modelo siguen en la caché.

**Estados del icono de la bandeja:**

| Icono | Estado |
|:---:|---|
| anillo blanco | modelos cargados, listo |
| disco rojo | grabando |
| disco naranja | transcribiendo |
| anillo gris tachado (⊘) | modelos descargados, memoria GPU liberada, dictado desactivado |
| arco azul de tres cuartos | recargando los modelos |

Mientras los modelos están descargados, un clic en el icono o una pulsación de atajo muestra una notificación de error en lugar de grabar.

**A mano:** clic derecho en el icono → *Unload models (free GPU memory)* (descargar los modelos), y después *Reload models* (recargar los modelos).

**Desde un agente de IA (MCP):** inicie el daemon con `-m` / `--mcp` (desactivado por defecto). `make` compila también `opons-voxd-mcp`, un servidor [Model Context Protocol](https://modelcontextprotocol.io) por stdio, enlazado solo con la libc (unos 1,5 MB de RAM), que expone tres herramientas: `unload_models`, `reload_models` y `get_status`. Con Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

La mayoría de los demás clientes usan este JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Detalles en el [README principal](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Atajo de pulsar para hablar

El `launch.sh` incluido usa `ctrl+alt+w` (recomendado en AZERTY) o `ctrl+alt+z` (recomendado en QWERTY). La tecla está junto a la `A`, así que la combinación entera se alcanza solo con la mano izquierda, y la mayoría de los entornos de escritorio no la tienen asignada por defecto. Se configura con `OPONS_VOXD_PTT_HOTKEY`. Todos los detalles en el [README principal](../README.md#push-to-talk-hotkey).

---

## Licencia

Todo bajo licencia MIT: modelo Whisper (OpenAI), whisper.cpp (Georgi Gerganov) y este proyecto.

Documentación completa: ver el [README principal](../README.md).

---

## Estilo de código

Este proyecto sigue el **estilo de codificación del kernel Linux** (`Documentation/process/coding-style.rst`) con adaptaciones de Olivier Pons: llaves K&R, indentación de 4 espacios, `struct name` sin typedef, sin prefijos artificiales (`s_`, `t_`, `e_`), `return value;` sin paréntesis, comentarios kernel-doc, máximo 80 caracteres por línea. Detalles completos en el [README principal](../README.md).
