# Mi flujo de trabajo: dictar en tu idioma, escribir en inglés

🌍 **Idiomas:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Mi lengua materna es el francés, pero casi todo mi trabajo de desarrollo —mensajes de commit, comentarios en el código, issues de GitHub, chats con equipos internacionales, documentación— se hace en inglés. Escribir un inglés fluido no es lo difícil; el verdadero cuello de botella es *pensar* en inglés mientras el resto del cerebro sigue funcionando en tu propio idioma. El dictado elimina por completo ese cuello de botella, siempre que no tengas que traducirte a ti mismo.

## Mi configuración

Uso opons-voxd con dos atajos push-to-talk independientes:

- **`Ctrl+Alt+W`: dictado normal.** Lo mantengo pulsado, hablo en francés, lo suelto y el texto en francés se escribe en el cursor. Para notas, chats con colegas francófonos y todo lo que deba quedarse en francés.
- **`Ctrl+Alt+K`: dictado con traducción.** El mismo gesto, pero hablo en francés y en el cursor aparece *inglés*. Para todo lo que va dirigido a un público angloparlante: mensajes de commit, descripciones de PR, comentarios en el código, issues de GitHub, canales de chat en inglés.

Nada de copiar y pegar en un traductor, nada de cambiar de contexto, nada de volver a escribir. Hablo, y el inglés aparece justo donde ya está mi cursor.

## Por qué dos atajos y no uno

La traducción solo sale bien con un modelo entrenado de verdad para esa tarea. Para el atajo diario en francés uso `large-v3-turbo` porque es rápido, pero traduce mal: se optimizó para la velocidad de transcripción, no para traducir. Por eso el atajo de traducción carga un segundo modelo (`medium`), que sí traduce bien. Los dos modelos están a la vez en la memoria de la GPU; en una tarjeta con más de unos pocos GB de margen, ese consumo adicional es insignificante. Cuando necesito recuperar esa memoria para una tarea de IA local, el agente de IA que la ejecuta descarga ambos modelos de la memoria a través del servidor MCP de opons-voxd y después vuelve a cargarlos; mientras tanto, el icono de la bandeja se convierte en un anillo gris tachado, así sé que el dictado está desactivado (consulta [Liberar la GPU para la IA local](README.es.md#liberar-la-gpu-para-la-ia-local-servidor-mcp)).

## Pruébalo

Todo se ajusta con la configuración normal de opons-voxd, sin fork ni parche. Las variables `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` y `OPONS_VOXD_LANGUAGE` se describen en las secciones [Configuration](../README.md#configuration) y [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) del README principal, en inglés.

Si en tu día a día hablas un idioma distinto del inglés pero trabajas en inglés, esta es probablemente la función más útil del proyecto.
