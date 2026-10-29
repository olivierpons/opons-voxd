# Meu fluxo de trabalho: dite no seu idioma, digite em inglês

🌍 **Idiomas:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Minha língua materna é o francês, mas a maior parte do meu trabalho com software — mensagens de commit, comentários de código, issues no GitHub, conversas com equipes internacionais, documentação — acontece em inglês. Digitar um inglês fluente não é a parte difícil; o verdadeiro gargalo é *pensar* em inglês enquanto o resto do cérebro funciona na língua materna. O ditado elimina esse gargalo por completo quando você não precisa traduzir a si mesmo.

## A configuração

Uso o opons-voxd com dois atalhos push-to-talk independentes:

- **`Ctrl+Alt+W` — ditado normal.** Mantenho pressionado, falo em francês, solto, e o texto em francês é digitado no cursor. Para anotações, conversas com colegas francófonos, tudo o que deve continuar em francês.
- **`Ctrl+Alt+K` — ditar e traduzir.** O mesmo gesto, mas falo em francês e é *inglês* que aparece no cursor. Para tudo o que se dirige a um público de língua inglesa: mensagens de commit, descrições de PR, comentários de código, issues no GitHub, canais de chat em inglês.

Nada de copiar e colar em um tradutor, nada de trocar de contexto, nada de redigitar. Eu falo, e o inglês aparece onde o meu cursor já está.

## Por que dois atalhos e não um

A tradução só funciona bem com um modelo realmente treinado para essa tarefa. No atalho do dia a dia, em francês, uso o `large-v3-turbo` porque é rápido — mas ele traduz mal, já que foi otimizado para a velocidade de transcrição, não para a tradução. O atalho de tradução carrega, em vez dele, um segundo modelo (`medium`), que traduz corretamente. Os dois modelos ficam na memória da GPU ao mesmo tempo; em uma placa com mais de alguns GB de folga, esse custo extra é irrelevante. Quando preciso dessa memória de volta para uma tarefa de IA local, o agente que a executa descarrega os dois modelos pelo servidor MCP do opons-voxd e os recarrega depois; enquanto isso, o ícone na bandeja vira um anel cinza cortado por uma barra, e assim sei que o ditado está desativado (veja [Liberar a GPU para IA local](README.pt.md#liberar-a-gpu-para-ia-local-servidor-mcp)).

## Experimente

É a configuração padrão do opons-voxd — sem fork nem patch. Veja [Configuration](../README.md#configuration) e [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) no README principal para `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` e `OPONS_VOXD_LANGUAGE`.

Se no dia a dia você fala outro idioma que não o inglês, mas trabalha em inglês, este é provavelmente o recurso mais útil deste projeto.
