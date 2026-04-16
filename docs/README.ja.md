# Voice In Linux

**Linux向けローカル音声テキスト変換ツール ── 音声データがマシンの外に出ることは一切ありません。**

🌍 **言語：**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [Español](README.es.md) · [Italiano](README.it.md)

---

## なぜこのプロジェクトを作ったのか

Linux上のあらゆるアプリケーションにテキストを音声入力したい。しかし、音声をサードパーティのサーバーに送信したくない。既存のソリューション（ブラウザ拡張機能、クラウドAPI、SaaSプラットフォーム）はすべて、音声をリモートサーバーにストリーミングして文字起こしを行います。つまり、話したすべての言葉がインターネットを経由します：メール、機密文書、顧客データ、個人メモ、すべてです。

このプロジェクトは、軽量で完全にローカルな代替手段です。[whisper.cpp](https://github.com/ggerganov/whisper.cpp)（OpenAIのオープンソース音声認識モデル）を使用して、完全にローカルマシン上で動作します。初回セットアップ後はネットワーク接続は不要です。データがコンピューターの外に出ることはありません。

AIの助けを借りて開発しました。

---

## 使い方

1. システムトレイに小さなアイコンが表示されます
2. **左クリック** → 録音開始（アイコンが赤に変化）
3. **再度左クリック** → 録音停止、Whisperによるローカル文字起こし
4. 文字起こしされたテキストが自動的にクリップボードにコピーされます（X11のPRIMARYとCLIPBOARD両方）
5. デスクトップ通知にテキストが10秒間表示されます
6. `Ctrl+Shift+V`、`Shift+Insert`、または中クリックで貼り付け

---

## 機能

- **100%ローカル** ── 音声はCPUまたはGPUで処理され、外部に送信されません
- **GPU高速化** ── NVIDIA CUDAサポートによるほぼ瞬時の文字起こし
- **99言語対応** ── OpenAI Whisperによる多言語サポート
- **軽量** ── 単一のCバイナリ（約100 KB）、Pythonは不要
- **システムトレイ統合** ── タスクバーの目立たないアイコン
- **デュアルクリップボード** ── PRIMARYとCLIPBOARDの両方にテキストをコピー
- **自動大文字化** ── 文頭が自動的に大文字になります
- **自動起動** ── ログイン時に自動起動を設定可能

---

## インストール

```bash
# 1. システム依存パッケージ
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libgtk-3-dev libnotify-dev libportaudio-dev libcairo2-dev \
    xclip libnotify-bin

# 2. リポジトリをクローン
git clone https://github.com/olivierpons/voice_in_linux.git
cd voice_in_linux

# 3. whisper.cppをビルドしモデルをダウンロード
make setup

# 4. バイナリをビルド
make

# 5. 実行
./voice_in
```

---

## ライセンス

すべてMITライセンスです：Whisperモデル（OpenAI）、whisper.cpp（Georgi Gerganov）、および本プロジェクト。

詳細なドキュメントは[メインREADME](../README.md)をご覧ください。
