# opons-voxd

> 注：opons-voxd は jakovius/voxd とは無関係です。名前は独自に付けたもので、`opons` は作者 O. Pons、`voxd` は vox + daemon（ラテン語と Unix の伝統にならった命名）に由来します。

> **⚠ 後方互換性は意図的に廃止しました。** 本プロジェクトの旧名は `voice_in_linux` です。名称変更に伴い、バイナリ名、ソースファイル名、すべての環境変数（`VOICE_IN_*` → `OPONS_VOXD_*`）が変わり、実行時の `xdotool` 依存もなくなりました（`libxtst-dev` による XTest での直接入力に置き換え）。エイリアスも移行期間もありません。起動スクリプト、自動起動の設定、`apt install` の行を更新してください。

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

2つの入力方法があり、同時に使えます。

**トレイアイコンモード（クリックで切り替え、クリップボードにコピー）**

1. システムトレイに小さなアイコンが表示されます
2. **左クリック** → 録音開始（アイコンが赤に変化）
3. **再度左クリック** → 録音停止、Whisperによるローカル文字起こし
4. 文字起こしされたテキストが自動的にクリップボードにコピーされます（X11のPRIMARYとCLIPBOARD両方）
5. デスクトップ通知にテキストが10秒間表示されます
6. `Ctrl+Shift+V`、`Shift+Insert`、または中クリックで貼り付け

アイコンを**右クリック**すると、録音の切り替え、モデルのアンロード、モデルの再読み込み、終了のメニューが開きます（後述の[GPUの解放](#ローカルai用にgpuを解放mcpサーバー)を参照）。

**プッシュ・トゥ・トークモード（ホットキーを押している間だけ録音し、カーソル位置に入力）**

1. **`Ctrl+Shift+Space` を押し続ける** → 録音開始（アイコンが赤に変化）
2. **キーを離す** → 録音停止、文字起こし
3. 文字起こしされたテキストが、X11 の XTest 拡張により**フォーカス中のウィンドウのカーソル位置に直接入力**されます

ホットキーは `OPONS_VOXD_PTT_HOTKEY` で変更できます。もう1つ独立したホットキー `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`（デフォルト `ctrl+alt+t`）は同じ操作で、英訳したテキストを入力します。

### スクリーンショット

| 待機中 | 録音中 |
|:---:|:---:|
| ![待機中](../screenshots/opons_voxd_inactive.png) | ![録音中](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **実用的な応答速度にはNVIDIA GPU + CUDAが不可欠です。**
> CUDAなし：15秒の音声に対して**10〜15秒**の処理時間。
> CUDAあり：同じ文字起こしが**1秒未満**。
> NVIDIA GPU（GTX 1060以降）をお持ちの場合、CUDAのセットアップを最優先にしてください。

---

## 機能

- **100%ローカル** ── 音声はCPUまたはGPUで処理され、外部に送信されません
- **GPU高速化** ── NVIDIA CUDAサポートによるほぼ瞬時の文字起こし
- **99言語対応** ── OpenAI Whisperによる多言語サポート（日本語、英語、フランス語、ドイツ語、中国語、スペイン語、イタリア語など）
- **軽量** ── 純粋なC言語、Python不要：デーモン本体と、libcのみにリンクする任意の25 KBのMCPサーバー
- **システムトレイ統合** ── タスクバーの目立たないアイコン
- **プッシュ・トゥ・トーク用ホットキー** ── `Ctrl+Shift+Space`（変更可能）を押している間録音し、離すとカーソル位置に文字起こし結果を入力
- **プッシュ・トゥ・トーク＋翻訳用ホットキー** ── 独立して設定できる2つ目のホットキーで、逐語的な文字起こしではなく英訳を入力
- **デュアルクリップボード** ── PRIMARYとCLIPBOARDの両方にテキストをコピー
- **デスクトップ通知** ── 文字起こし結果を通知で表示。通知はデフォルトで一時的で、表示後完全に消えます。`OPONS_VOXD_NOTIFY_PERSIST=1` で通知履歴に残せます。
- **通知モード** ── `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` で成功時の通知を制御します（デフォルトは `quiet`：吹き出しあり、音なし）。エラーはどのモードでも例外なく音付きで表示されます。一覧表は[メインREADME](../README.md#notifications)をご覧ください。
- **音声コマンド** ── 句読点や書式の組み込みコマンド。言語ごとのコマンドファイル（`commands/`）、デフォルトで無効（`OPONS_VOXD_COMMANDS=1` で有効化）。新言語追加は `commands/xx.txt` を作成するだけ、再コンパイル不要。
- **自動大文字化** ── 文頭が自動的に大文字になります
- **必要に応じてGPUを解放** ── WhisperモデルをVRAMからアンロードし（トレイメニューから、または内蔵MCPサーバー経由で任意のAIエージェントから）、作業後に再読み込み
- **自動起動** ── ログイン時に自動起動を設定可能

---

## インストール

```bash
# 1. システム依存パッケージ
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. リポジトリをクローン
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. whisper.cppをビルドしモデルをダウンロード
make setup

# 4. opons-voxd（デーモン）と opons-voxd-mcp（任意のMCPサーバー）をビルド
make

# 5. 実行
./opons-voxd
```

---

## ライセンス

すべてMITライセンスです：Whisperモデル（OpenAI）、whisper.cpp（Georgi Gerganov）、および本プロジェクト。

詳細なドキュメントは[メインREADME](../README.md)をご覧ください。

---

## コーディングスタイル

本プロジェクトは **Linuxカーネルコーディングスタイル**（`Documentation/process/coding-style.rst`）に従い、Olivier Pons による調整を加えています：K&R波括弧、4スペースインデント、typedef なしの `struct name`、人為的プレフィックスなし（`s_`、`t_`、`e_`）、括弧なしの `return value;`、kernel-docコメント、1行最大80文字。詳細は[メインREADME](../README.md)をご覧ください。

---

## プッシュ・トゥ・トーク用ホットキー

同梱の `launch.sh` は `ctrl+alt+w`（AZERTY配列に推奨）または `ctrl+alt+z`（QWERTY配列に推奨）を使います。このキーは `A` のすぐ隣にあるため、左手だけで組み合わせ全体を押せ、ほとんどのデスクトップ環境ではデフォルトで割り当てられていません。設定は `OPONS_VOXD_PTT_HOTKEY` で行います。詳しくは[メインREADME](../README.md#push-to-talk-hotkey)をご覧ください。

---

## ローカルAI用にGPUを解放（MCPサーバー）

Whisperモデルは常にGPUメモリに常駐しています。ローカルLLM、画像生成モデル、学習処理などでそのVRAMが必要なとき、opons-voxd は終了せずにモデルをアンロードし、後で再読み込みできます。アンロード時にはプロセスのCUDAコンテキストも破棄されるため、opons-voxd は `nvidia-smi` から完全に消えます（VRAM使用量0 MB）。モデルファイルがページキャッシュに残っていれば、再読み込みは1秒未満で終わります。

**トレイアイコンの状態**：

| アイコン | 状態 |
|:---:|---|
| 白いリング | モデル読み込み済み、準備完了 |
| 赤い円 | 録音中 |
| オレンジの円 | 文字起こし中 |
| 斜線入りの灰色リング（⊘） | モデルをアンロード済み、GPUメモリ解放済み、音声入力は無効 |
| 青い4分の3の円弧 | モデル再読み込み中 |

アンロード中にアイコンをクリックしたりプッシュ・トゥ・トーク用ホットキーを押したりすると、録音は始まらず、エラー通知が表示されます。

**手動で操作する場合**：トレイアイコンを右クリック > *Unload models (free GPU memory)*（モデルをアンロード）、その後 *Reload models*（モデルを再読み込み）。

**AIエージェントから操作する場合（MCP）**：デーモンを `-m` / `--mcp` 付きで起動します（デフォルトは無効）。`make` は `opons-voxd-mcp` もビルドします。これはstdio経由の [Model Context Protocol](https://modelcontextprotocol.io) サーバーで、libcのみにリンクし、1プロセスあたりのメモリ使用量は約1.5 MBです。提供するツールは `unload_models`、`reload_models`、`get_status` の3つです。

Claude Code での登録：

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

ほかの多くのクライアント（Claude Desktop、Cursor、LM Studio など）は同じ形式のJSONを使います：

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

詳しくは[メインREADME](../README.md#free-the-gpu-for-local-ai-mcp-server)をご覧ください。
