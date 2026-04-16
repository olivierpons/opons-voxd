# Voice In Linux

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

1. Un icono aparece en la bandeja del sistema
2. **Clic izquierdo** → comienza la grabación (el icono se vuelve rojo)
3. **Clic izquierdo de nuevo** → se detiene, el audio se transcribe localmente con Whisper
4. El texto transcrito se copia automáticamente al portapapeles (ambas selecciones X11)
5. Una notificación muestra el texto durante 10 segundos
6. Pegue con `Ctrl+Shift+V`, `Shift+Insert` o clic central

---


### Screenshots

| Inactivo | Grabando |
|:---:|:---:|
| ![Inactivo](../screenshots/voice_in_inactive.png) | ![Grabando](../screenshots/voice_in_active.png) |

## Características

- **100% local** — el audio se procesa en CPU o GPU, nunca se envía
- **Aceleración GPU** — soporte NVIDIA CUDA para transcripción casi instantánea
- **99 idiomas** — impulsado por OpenAI Whisper
- **Ligero** — binario C único (~100 KB), sin Python ni dependencias de ejecución
- **Integración en la bandeja** — icono discreto en la barra de tareas
- **Doble portapapeles** — texto copiado en PRIMARY y CLIPBOARD
- **Mayúsculas automáticas** — las oraciones se capitalizan automáticamente
- **Inicio automático** — configurable al iniciar sesión

---

## Instalación

```bash
# 1. Dependencias del sistema
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin

# 2. Clonar el repositorio
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux

# 3. Compilar whisper.cpp y descargar el modelo
make setup

# 4. Compilar el binario
make

# 5. Ejecutar
./voice_in
```

---

## Licencia

Todo bajo licencia MIT: modelo Whisper (OpenAI), whisper.cpp (Georgi Gerganov) y este proyecto.

Documentación completa: ver el [README principal](../README.md).
