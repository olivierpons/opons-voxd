# 我的工作流程：用母语口述，输出英文

🌍 **翻译**：
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

我的母语是法语，但我的软件工作大多用英语进行：提交信息、代码注释、GitHub issue、与国际团队的聊天、文档，全是英语。用英语流利地打字并不难；真正的瓶颈在于，大脑的其余部分还在用母语运转，却要用英语*思考*。只要不必自己翻译，口述就能彻底消除这个瓶颈。

## 配置

我给 opons-voxd 设了两个相互独立的一键通话快捷键：

- **`Ctrl+Alt+W`：普通口述**。按住，说法语，松开，法语文字就输入到光标处。用于笔记、和讲法语的同事聊天，以及一切要保留法语的内容。
- **`Ctrl+Alt+K`：口述并翻译**。操作相同，但我说的是法语，光标处出现的却是*英文*。用于一切面向英语读者的内容：提交信息、PR 描述、代码注释、GitHub issue、英语聊天频道。

不用复制粘贴到翻译工具，不用切换思路，也不用重新打字。我一开口，英文就出现在光标所在的位置。

## 为什么是两个快捷键，而不是一个

只有真正针对翻译任务训练过的模型，才能把翻译做好。日常的法语快捷键我用 `large-v3-turbo`，因为它快——但它是为转写速度而优化的，不是为翻译，所以翻译质量很差。因此，翻译快捷键会加载第二个模型（`medium`），由它来完成准确的翻译。两个模型同时驻留在 GPU 显存中；只要显卡还有几 GB 以上的余量，这点额外开销根本不成问题。当本地 AI 任务需要腾出这部分显存时，运行该任务的智能体会通过 opons-voxd 的 MCP 服务器卸载两个模型，任务结束后再重新加载；在此期间，托盘图标会变成带斜线的灰色圆环，我一看就知道语音输入已停用（参见[为本地 AI 释放 GPU](README.zh.md#为本地-ai-释放-gpumcp-服务器)）。

## 试一试

这是 opons-voxd 的标准配置，不需要 fork，也不需要打补丁。关于 `OPONS_VOXD_PTT_HOTKEY`、`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`、`OPONS_VOXD_TRANSLATE_MODEL` 和 `OPONS_VOXD_LANGUAGE`，请参见主 README 中的 [Configuration](../README.md#configuration) 和 [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey)。

如果你日常说的是英语以外的语言，工作却要用英语，这大概就是本项目最实用的功能。
