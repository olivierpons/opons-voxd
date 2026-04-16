# Voice In Linux

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

1. 系统托盘中出现一个小图标
2. **左键单击** → 开始录音（图标变红）
3. **再次左键单击** → 停止录音，通过 Whisper 在本地进行转录
4. 转录文本自动复制到剪贴板（X11 的 PRIMARY 和 CLIPBOARD 两种选区）
5. 桌面通知显示文本 10 秒
6. 使用 `Ctrl+Shift+V`、`Shift+Insert` 或鼠标中键粘贴

---

## 功能特点

- **100% 本地运行** ── 音频在 CPU 或 GPU 上处理，绝不外传
- **GPU 加速** ── 支持 NVIDIA CUDA，实现近乎即时的转录
- **99 种语言** ── 基于 OpenAI Whisper
- **轻量级** ── 单个 C 语言二进制文件（约 100 KB），无需 Python
- **系统托盘集成** ── 任务栏中的小图标
- **双剪贴板** ── 文本同时写入 PRIMARY 和 CLIPBOARD
- **自动大写** ── 句首字母自动大写
- **开机自启** ── 可配置为登录时自动启动

---

## 安装

```bash
# 1. 系统依赖
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin

# 2. 克隆项目
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux

# 3. 编译 whisper.cpp 并下载模型
make setup

# 4. 编译二进制文件
make

# 5. 运行
./voice_in
```

---

## 许可证

全部采用 MIT 许可证：Whisper 模型（OpenAI）、whisper.cpp（Georgi Gerganov）以及本项目。

完整文档请参阅[主 README](../README.md)。
