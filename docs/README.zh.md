# opons-voxd

> 注：opons-voxd 与 jakovius/voxd 没有任何关系。这个名字是独立选定的：`opons` 即作者 O. Pons，`voxd` 即 vox + daemon（沿袭拉丁语和 Unix 的命名传统）。

> **⚠ 有意放弃向后兼容**。本项目原名 `voice_in_linux`。更名涉及二进制文件名、源文件名、所有环境变量（`VOICE_IN_*` → `OPONS_VOXD_*`），并去掉了运行时对 `xdotool` 的依赖（改为通过 `libxtst-dev` 直接用 XTest 合成按键）。没有别名，也没有过渡期，请更新你的启动脚本、开机自启动项和 `apt install` 命令行。

**Linux 本地语音输入，你的声音绝不会离开你的电脑。**

🌍 **翻译**：
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[我的工作流程：用母语口述，输出英文](WORKFLOW.zh.md)**（真实使用案例）

---

## 项目缘起

我想在 Linux 上向任意应用程序口述输入文字，又不想把自己的声音发送到第三方服务器。可我找到的现有方案，无论是浏览器扩展、云端 API 还是 SaaS 平台，都会把音频流式传输到远程服务器上转写。这意味着你说的每一个字都要经过互联网：邮件、机密文件、客户数据、私人笔记，无一例外。

本项目是一个轻量、完全本地的替代方案。它借助运行 OpenAI 开源语音识别模型的 [whisper.cpp](https://github.com/ggerganov/whisper.cpp)，完全在你自己的电脑上运行。完成初始设置后无需任何网络连接，数据永远不会离开你的电脑。

本项目在 AI 的协助下完成。

---

## 工作原理

有两种口述方式，可以同时使用。

### 托盘图标模式（点击切换，复制到剪贴板）

1. 系统托盘中出现一个小图标
2. **左键单击** → 开始录音（图标变红）
3. **再次左键单击** → 停止录音，由 Whisper 在本地转写音频
4. 转写出的文字自动复制到剪贴板（X11 的两个选区都会写入）
5. 桌面通知显示该文字 10 秒
6. 用 `Ctrl+Shift+V`、`Shift+Insert` 或鼠标中键粘贴到任意位置

**右键单击**图标可打开菜单：Toggle / Unload models / Reload models / Quit（参见[为本地 AI 释放 GPU](#为本地-ai-释放-gpumcp-服务器)）。

### 一键通话模式（按住快捷键，在光标处输入）

1. **按住 `Ctrl+Shift+Space`** → 开始录音（图标变红）
2. **松开按键** → 停止录音，转写音频
3. 转写出的文字会**直接输入到键盘光标处**，即当前获得焦点的窗口中（通过 X11 的 XTest 扩展实现）

快捷键可以通过 `OPONS_VOXD_PTT_HOTKEY` 配置。另有一个相互独立的第二快捷键（`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`，默认为 `ctrl+alt+t`），用法相同，但输入的是英文译文，详见[主 README](../README.md#push-to-talk-and-translate-hotkey)。

### 截图

| 空闲 | 录音中 |
|:---:|:---:|
| ![opons-voxd 空闲](../screenshots/opons_voxd_inactive.png) | ![opons-voxd 录音中](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **要获得可用的响应速度，支持 CUDA 的 NVIDIA GPU 至关重要**。
> 没有 CUDA 时，15 秒的语音需要 **10～15 秒**才能转写完（仅用 CPU）。
> 启用 CUDA 后，同样的转写**不到 1 秒**即可完成。

---

## 功能特性

- **100% 本地**：音频在你的 CPU 或 GPU 上处理，绝不外传
- **GPU 加速**：支持 NVIDIA CUDA，转写几乎即时完成
- **99 种语言**：由 OpenAI Whisper 提供支持
- **轻量**：纯 C 编写，不依赖 Python；只有守护进程本身，外加一个可选的、仅链接 libc 的 25 KB MCP 服务器
- **系统托盘集成**：任务栏中一个不打扰的图标
- **一键通话快捷键**：按住 `Ctrl+Shift+Space`（可配置）录音，松开后转写文字即输入到键盘光标处
- **一键通话并翻译快捷键**：第二个可独立配置的快捷键，输入的不是逐字转写，而是你所说内容的英文译文
- **双剪贴板**：文字同时写入 X11 的 PRIMARY 和 CLIPBOARD 两个选区
- **桌面通知**：以通知形式显示转写出的文字
- **语音命令**：`commands/` 中每种语言对应一个命令文件，默认关闭（`OPONS_VOXD_COMMANDS=1`）。新建 `commands/xx.txt` 即可添加一种语言，无需重新编译。
- **临时通知**：默认情况下，通知显示后会彻底消失。设置 `OPONS_VOXD_NOTIFY_PERSIST=1` 可将其保留在通知历史中。
- **通知模式**：`OPONS_VOXD_NOTIFY=normal|quiet|silent|off` 控制成功时的通知（默认为 `quiet`：显示气泡，不发声）。错误在任何模式下都会带声音显示，没有例外。完整对照表见主 README。
- **自动大写**：自动将句首字母大写
- **按需释放 GPU**：将 Whisper 模型从显存中卸载（通过托盘菜单，或由任意 AI 智能体通过内置的 MCP 服务器操作），用完后再重新加载
- **开机自启动**：可配置为登录时自动启动

---

## 环境要求

### 操作系统

| 要求 | 最低版本 | 检查方式 |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Linux 内核 | 5.15+ | `uname -r` |
| 显示服务器 | X11 | `echo $XDG_SESSION_TYPE` |

> 不支持 Wayland（GtkStatusIcon 需要 X11）。Linux Mint 默认使用 X11。

### 构建工具（必需）

| 软件包 | 最低版本 | 安装 |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### 开发库（必需）

| 软件包 | 最低版本 | 安装 |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> 不需要 `libportaudio-dev`：构建时会获取 `portaudio.h`（首次 `make` 时通过 `curl` 下载），并直接链接 `libportaudio.so.2`。详见[主 README](../README.md#development-libraries-required)。

### 运行时工具（必需）

| 工具 | 用途 | 安装 |
|---|---|---|
| xclip | 复制到 X11 剪贴板 | `sudo apt install xclip` |
| notify-send | 桌面通知 | `sudo apt install libnotify-bin` |

### NVIDIA GPU（可选，但对性能至关重要）

没有 CUDA 时，处理 15 秒的语音预计需要 **10～15 秒**；有 CUDA 时**不到 1 秒**，提速 **10～50 倍**。

| 组件 | 最低版本 | 检查方式 |
|---|---|---|
| NVIDIA 驱动 | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

如果 `nvidia-smi` 可用而 `nvcc` 不可用，请安装 CUDA 工具包：

```bash
sudo apt install nvidia-cuda-toolkit
```

如果 `nvidia-smi` 显示的 CUDA 版本**低于** `nvcc` 报告的版本，请更新驱动：

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## 安装

```bash
# 1. 系统依赖
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. 克隆仓库
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. 编译 whisper.cpp 并下载模型
make setup

# 4. 编译二进制程序：opons-voxd（守护进程）和 opons-voxd-mcp（可选的 MCP 服务器）
make

# 5. 运行
./opons-voxd
```

---

## 为本地 AI 释放 GPU（MCP 服务器）

Whisper 模型始终驻留在 GPU 显存中（`large-v3-turbo` + `medium` 约占 3.8 GB）。当你需要把这部分显存用在别处，比如本地 LLM、图像模型或一次训练任务，opons-voxd 可以在不退出的情况下卸载模型，事后再重新加载。卸载还会销毁进程的 CUDA 上下文，因此 opons-voxd 会从 `nvidia-smi` 中彻底消失（显存占用 0 MB）。如果模型文件仍在页缓存中，重新加载不到一秒；否则需要几秒。

**托盘图标状态**：

| 图标 | 状态 |
|:---:|---|
| 白色圆环 | 模型已加载，就绪 |
| 红色圆点 | 正在录音 |
| 橙色圆点 | 正在转写 |
| 带斜线的灰色圆环（⊘） | 模型已卸载，GPU 显存已释放，语音输入已停用 |
| 蓝色四分之三圆弧 | 正在重新加载模型 |

模型卸载期间，点击图标或按下一键通话快捷键不会开始录音，而是弹出错误通知。

**手动操作**：右键单击托盘图标，选择 *Unload models (free GPU memory)*（卸载模型，释放 GPU 显存），之后再选择 *Reload models*（重新加载模型）。

**由 AI 智能体操作（MCP）**：用 `-m` / `--mcp` 启动守护进程（默认关闭）。`make` 还会构建 `opons-voxd-mcp`，这是一个基于 stdio 的 [Model Context Protocol](https://modelcontextprotocol.io) 服务器，仅链接 libc（内存占用约 1.5 MB）。它提供三个工具：`unload_models`、`reload_models` 和 `get_status`。在 Claude Code 中：

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

其他大多数客户端（Claude Desktop、Cursor、LM Studio 等）使用如下 JSON：

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

完整说明见[主 README](../README.md#free-the-gpu-for-local-ai-mcp-server)。

---

## 一键通话快捷键

自带的 `launch.sh` 使用 `ctrl+alt+w`（推荐 AZERTY 键盘使用）或 `ctrl+alt+z`（推荐 QWERTY 键盘使用）。这个键紧挨着 `A`，整个组合仅用左手就能按到（小指按 `Ctrl`，拇指按 `Alt`，无名指或中指按 `W`/`Z`），无需打乱打字姿势，而且在大多数桌面环境中默认没有被占用。可通过 `OPONS_VOXD_PTT_HOTKEY` 进行配置。完整说明见[主 README](../README.md#push-to-talk-hotkey)。

---

## 许可证

全部采用 MIT 许可证：Whisper 模型（OpenAI）、whisper.cpp（Georgi Gerganov）以及本项目。

完整文档请参阅[主 README](../README.md)。

---

## 代码风格

本项目遵循 **Linux 内核代码风格**（`Documentation/process/coding-style.rst`），并由 Olivier Pons 做了调整：K&R 风格的大括号、4 空格缩进、不用 typedef 的 `struct name`、不加前缀（`s_`、`t_`、`e_`）、不带括号的 `return value;`、kernel-doc 注释、每行最多 80 个字符。完整说明见[主 README](../README.md)。
