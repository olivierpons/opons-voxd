# opons-voxd

> 注：opons-voxd 与 jakovius/voxd 毫无关系，名称系独立选定：`opons` 取自作者 O. Pons，`voxd` 即 vox + daemon（沿袭拉丁语与 Unix 的命名传统）。

> **⚠ 有意不保持向后兼容。** 本项目原名 `voice_in_linux`。此次更名涉及二进制文件名、源文件名、所有环境变量（`VOICE_IN_*` → `OPONS_VOXD_*`），并移除了运行时对 `xdotool` 的依赖（改为通过 `libxtst-dev` 直接用 XTest 合成按键）。不提供别名，也没有过渡期，请相应更新您的启动脚本、自启动项和 `apt install` 命令。

**Linux 本地语音转文字工具 ── 您的声音永远不会离开您的电脑。**

🌍 **语言：**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md)

---

## 为什么做这个项目？

我希望能在 Linux 上对任何应用进行语音输入，而不需要将音频发送到第三方服务器。现有的所有方案——浏览器扩展、云端 API、SaaS 平台——都会将您的音频流式传输到远程服务器进行转录。这意味着您说出的每一个字都会经过互联网传输：邮件、机密文件、客户数据、个人笔记，全部如此。

本项目是一个轻量级的、完全本地化的替代方案。它完全运行在您的本地机器上，使用 [whisper.cpp](https://github.com/ggerganov/whisper.cpp)——OpenAI 开源的语音识别模型。初始安装完成后无需任何网络连接，数据永远不会离开您的电脑。

借助人工智能开发完成。

---

## 工作原理

两种听写方式，可同时使用：

**托盘图标模式（单击切换，复制到剪贴板）**

1. 系统托盘中出现一个小图标
2. **左键单击** → 开始录音（图标变红）
3. **再次左键单击** → 停止录音，通过 Whisper 在本地进行转录
4. 转录文本自动复制到剪贴板（X11 的 PRIMARY 和 CLIPBOARD 两种选区）
5. 桌面通知显示文本 10 秒
6. 使用 `Ctrl+Shift+V`、`Shift+Insert` 或鼠标中键粘贴

**右键单击**图标可打开菜单：切换录音、卸载模型、重新加载模型、退出（参见下文[释放 GPU](#释放-gpu-供本地-ai-使用mcp-服务器)）。

**按键通话模式（按住快捷键，在光标处直接输入）**

1. **按住 `Ctrl+Shift+Space`** → 开始录音（图标变红）
2. **松开按键** → 停止录音并转录
3. 转录文本通过 X11 XTest 扩展**直接输入到当前焦点窗口的光标处**

快捷键可通过 `OPONS_VOXD_PTT_HOTKEY` 修改。另有一个独立的快捷键 `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`（默认 `ctrl+alt+t`），用法相同，但输入的是英文译文。

### 截图

| 空闲 | 录音中 |
|:---:|:---:|
| ![空闲](../screenshots/opons_voxd_inactive.png) | ![录音中](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **NVIDIA GPU + CUDA 对于可接受的响应时间至关重要。**
> 没有 CUDA：15 秒语音需要 **10-15 秒**处理。
> 使用 CUDA：同样的转录**不到 1 秒**。
> 如果您有 NVIDIA GPU（GTX 1060 或更新），请优先配置 CUDA。

---

## 功能特点

- **100% 本地运行** ── 音频在 CPU 或 GPU 上处理，绝不外传
- **GPU 加速** ── 支持 NVIDIA CUDA，实现近乎即时的转录
- **99 种语言** ── 基于 OpenAI Whisper（中文、英语、法语、德语、日语、西班牙语、意大利语等）
- **轻量级** ── 纯 C 语言，无需 Python：一个守护进程，外加一个可选的 25 KB MCP 服务器（仅链接 libc）
- **系统托盘集成** ── 任务栏中的小图标，不打扰工作
- **按键通话快捷键** ── 按住 `Ctrl+Shift+Space`（可自定义）录音，松开后转录文本直接输入到光标处
- **按键通话并翻译快捷键** ── 第二个可独立配置的快捷键，输入的是语音的英文译文，而非逐字转录
- **双剪贴板** ── 文本同时写入 PRIMARY 和 CLIPBOARD
- **桌面通知** ── 以通知形式显示转录文本。默认为瞬态通知，出现后自动消失；设置 `OPONS_VOXD_NOTIFY_PERSIST=1` 可保留在通知历史中。
- **通知模式** ── `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` 控制成功通知的显示方式（默认 `quiet`：显示气泡，无提示音）。错误通知在任何模式下都会带声音显示，没有例外。完整对照表见[主 README](../README.md#notifications)。
- **语音命令** ── 内置标点与格式命令，每种语言一个命令文件（`commands/` 目录），默认禁用（`OPONS_VOXD_COMMANDS=1` 启用）。添加新语言只需创建 `commands/xx.txt`，无需重新编译。
- **自动大写** ── 句首字母自动大写
- **按需释放 GPU** ── 可将 Whisper 模型从显存中卸载（通过托盘菜单，或由任意 AI 代理通过内置 MCP 服务器操作），用完后再重新加载
- **开机自启** ── 可配置为登录时自动启动

---

## 安装

```bash
# 1. 系统依赖
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. 克隆项目
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. 编译 whisper.cpp 并下载模型
make setup

# 4. 编译 opons-voxd（守护进程）和 opons-voxd-mcp（可选的 MCP 服务器）
make

# 5. 运行
./opons-voxd
```

---

## 许可证

全部采用 MIT 许可证：Whisper 模型（OpenAI）、whisper.cpp（Georgi Gerganov）以及本项目。

完整文档请参阅[主 README](../README.md)。

---

## 代码风格

本项目遵循 **Linux 内核编码风格**（`Documentation/process/coding-style.rst`），由 Olivier Pons 进行适配：K&R 花括号风格、4 空格缩进、直接使用 `struct name`（不使用 typedef）、无人为前缀（`s_`、`t_`、`e_`）、`return value;` 不加括号、kernel-doc 注释风格、每行最多 80 个字符。完整详情请参阅[主 README](../README.md)。

---

## 按键通话快捷键

附带的 `launch.sh` 使用 `ctrl+alt+w`（推荐 AZERTY 键盘）或 `ctrl+alt+z`（推荐 QWERTY 键盘）。该键紧挨着 `A`，仅用左手即可按下整个组合，而且在大多数桌面环境中默认未被占用。可通过 `OPONS_VOXD_PTT_HOTKEY` 配置。完整说明请参阅[主 README](../README.md#push-to-talk-hotkey)。

---

## 释放 GPU 供本地 AI 使用（MCP 服务器）

Whisper 模型会一直驻留在显存中。当您需要这部分显存运行本地大模型、图像模型或训练任务时，opons-voxd 无需退出即可卸载模型，事后再重新加载。卸载时还会销毁进程的 CUDA 上下文，因此 opons-voxd 会从 `nvidia-smi` 中彻底消失（显存占用 0 MB）。若模型文件仍在页缓存中，重新加载不到 1 秒。

**托盘图标状态**：

| 图标 | 状态 |
|:---:|---|
| 白色圆环 | 模型已加载，就绪 |
| 红色圆点 | 录音中 |
| 橙色圆点 | 转录中 |
| 灰色斜杠圆环（⊘） | 模型已卸载，显存已释放，听写不可用 |
| 蓝色四分之三圆弧 | 模型重新加载中 |

模型卸载期间，单击图标或按下按键通话快捷键不会录音，而是弹出错误通知。

**手动操作**：右键单击托盘图标 > *Unload models (free GPU memory)*（卸载模型），之后选 *Reload models*（重新加载模型）。

**由 AI 代理操作（MCP）**：启动守护进程时加上 `-m` / `--mcp`（默认关闭）。`make` 同时会编译 `opons-voxd-mcp`：一个基于 stdio 的 [Model Context Protocol](https://modelcontextprotocol.io) 服务器，仅链接 libc，每个实例约占 1.5 MB 内存。它提供三个工具：`unload_models`、`reload_models`、`get_status`。

在 Claude Code 中注册：

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

其他大多数客户端（Claude Desktop、Cursor、LM Studio 等）使用相同的 JSON 格式：

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

详情请参阅[主 README](../README.md#free-the-gpu-for-local-ai-mcp-server)。
