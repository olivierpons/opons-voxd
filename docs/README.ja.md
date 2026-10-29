# opons-voxd

> 注：opons-voxd は jakovius/voxd とは無関係です。名前は独自に決めたもので、`opons` は作者の O. Pons、`voxd` は vox + daemon（ラテン語と Unix の命名の伝統）に由来します。

> **⚠ 後方互換性はあえて断ち切っています**。本プロジェクトの旧名は `voice_in_linux` です。名称変更により、バイナリ名、ソースファイル名、すべての環境変数（`VOICE_IN_*` → `OPONS_VOXD_*`）が変わり、実行時の `xdotool` への依存もなくなりました（`libxtst-dev` を使った XTest による直接のキー入力合成に置き換え）。エイリアスも移行期間もありません。起動スクリプト、自動起動の設定、`apt install` の行を書き換えてください。

**Linux 向けのローカル音声入力。あなたの声がマシンの外に出ることはありません。**

🌍 **翻訳**：
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[私のワークフロー：母語で話し、英語で入力する](WORKFLOW.ja.md)**（実際の使用例）

---

## 開発の動機

Linux 上のどんなアプリケーションにも声でテキストを入力したい、ただし自分の声を第三者のサーバーには送りたくない、というのが出発点でした。見つかった既存のソリューション（ブラウザー拡張機能、クラウド API、SaaS プラットフォーム）は、どれも文字起こしのために音声をリモートサーバーへストリーミングします。つまり、話した言葉はすべてインターネットを経由します。メールも、機密文書も、顧客データも、個人的なメモも、すべてです。

本プロジェクトは、軽量で完全にローカルな代替手段です。OpenAI のオープンソース音声認識モデルを動かす [whisper.cpp](https://github.com/ggerganov/whisper.cpp) を使い、すべてがお使いのマシン上で動作します。初期セットアップが済めばネットワーク接続は不要で、データがコンピューターの外に出ることは一切ありません。

AI の助けを借りて開発しました。

---

## 仕組み

入力方法は 2 つあり、同時に使えます。

### トレイアイコンモード（クリックで切り替え、クリップボードにコピー）

1. システムトレイに小さなアイコンが表示されます
2. **左クリック** → 録音開始（アイコンが赤に変わる）
3. **もう一度左クリック** → 録音停止、Whisper で音声をローカルに文字起こし
4. 文字起こしされたテキストが自動的にクリップボードにコピーされます（X11 の両方のセレクション）
5. デスクトップ通知にテキストが 10 秒間表示されます
6. `Ctrl+Shift+V`、`Shift+Insert`、または中クリックで、どこにでも貼り付けられます

アイコンを**右クリック**すると、Toggle / Unload models / Reload models / Quit のメニューが開きます（[ローカル AI のための GPU 解放](#ローカル-ai-のための-gpu-解放mcp-サーバー)を参照）。

### プッシュ・トゥ・トークモード（ホットキーを押している間だけ録音し、カーソル位置に入力）

1. **`Ctrl+Shift+Space` を押し続ける** → 録音開始（アイコンが赤に変わる）
2. **キーを離す** → 録音停止、音声を文字起こし
3. 文字起こしされたテキストが、フォーカスのあるウィンドウの**キーボードカーソル位置に直接入力**されます（X11 の XTest 拡張機能を使用）

ホットキーは `OPONS_VOXD_PTT_HOTKEY` で変更できます。独立した 2 つ目のホットキー（`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`、デフォルトは `ctrl+alt+t`）も同じように動作しますが、こちらは英語に翻訳したテキストを入力します。詳しくは[メイン README](../README.md#push-to-talk-and-translate-hotkey)を参照してください。

### スクリーンショット

| 待機中 | 録音中 |
|:---:|:---:|
| ![opons-voxd 待機中](../screenshots/opons_voxd_inactive.png) | ![opons-voxd 録音中](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **実用的な応答時間には CUDA 対応の NVIDIA GPU が不可欠です**。
> CUDA なしでは、15 秒の音声の文字起こしに **10～15 秒**かかります（CPU のみ）。
> CUDA を有効にすると、同じ文字起こしが **1 秒未満**で終わります。

---

## 機能

- **100% ローカル**：音声は CPU または GPU 上で処理され、外部には一切送信されません
- **GPU アクセラレーション**：NVIDIA CUDA に対応し、ほぼ一瞬で文字起こし
- **99 言語に対応**：OpenAI Whisper による認識
- **軽量**：Python を使わない純粋な C。デーモン本体と、libc のみにリンクした 25 KB の MCP サーバー（任意）だけで構成
- **システムトレイとの統合**：タスクバーに控えめなアイコンを表示
- **プッシュ・トゥ・トークのホットキー**：`Ctrl+Shift+Space`（変更可能）を押している間に録音し、離すとキーボードカーソル位置に文字起こし結果を入力
- **翻訳付きプッシュ・トゥ・トークのホットキー**：独立して設定できる 2 つ目のホットキーで、話した内容をそのまま文字起こしする代わりに英語に翻訳して入力
- **2 つのクリップボード**：テキストを X11 の PRIMARY と CLIPBOARD の両方のセレクションに送信
- **デスクトップ通知**：文字起こしされたテキストを通知で表示
- **音声コマンド**：言語ごとに 1 つのコマンドファイルを `commands/` に配置。デフォルトでは無効（`OPONS_VOXD_COMMANDS=1` で有効化）。`commands/xx.txt` を作成するだけで言語を追加でき、再コンパイルは不要です。
- **一時的な通知**：デフォルトでは、通知は表示されたあと完全に消えます。`OPONS_VOXD_NOTIFY_PERSIST=1` にすると通知履歴に残ります。
- **通知モード**：`OPONS_VOXD_NOTIFY=normal|quiet|silent|off` で成功時の通知を制御します（デフォルトは `quiet`：吹き出しのみで音なし）。エラーはどのモードでも例外なく、必ず音付きで表示されます。完全な一覧表はメイン README を参照してください。
- **自動大文字化**：文頭を自動的に大文字にします
- **必要なときに GPU を解放**：Whisper のモデルを VRAM からアンロードし（トレイメニューから、または内蔵の MCP サーバー経由で任意の AI エージェントから）、用が済んだら再読み込み
- **自動起動**：ログイン時に起動するよう設定可能

---

## 必要環境

### オペレーティングシステム

| 要件 | 最小バージョン | 確認方法 |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Linux カーネル | 5.15+ | `uname -r` |
| ディスプレイサーバー | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland には対応していません（GtkStatusIcon が X11 を必要とするため）。Linux Mint はデフォルトで X11 を使用します。

### ビルドツール（必須）

| パッケージ | 最小バージョン | インストール |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### 開発ライブラリ（必須）

| パッケージ | 最小バージョン | インストール |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` は不要です。ビルド時に `portaudio.h` を取得し（最初の `make` 時に `curl` で）、`libportaudio.so.2` に直接リンクします。詳しくは[メイン README](../README.md#development-libraries-required)を参照してください。

### 実行時ツール（必須）

| ツール | 用途 | インストール |
|---|---|---|
| xclip | X11 クリップボードへのコピー | `sudo apt install xclip` |
| notify-send | デスクトップ通知 | `sudo apt install libnotify-bin` |

### NVIDIA GPU（任意だが性能には不可欠）

CUDA なしでは、15 秒の音声の処理に **10～15 秒**かかります。CUDA があれば **1 秒未満**です。**10～50 倍**の高速化になります。

| コンポーネント | 最小バージョン | 確認方法 |
|---|---|---|
| NVIDIA ドライバー | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

`nvidia-smi` は動くのに `nvcc` が動かない場合は、CUDA Toolkit をインストールしてください。

```bash
sudo apt install nvidia-cuda-toolkit
```

`nvidia-smi` が表示する CUDA のバージョンが、`nvcc` の報告するバージョンより**低い**場合は、ドライバーを更新してください。

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## インストール

```bash
# 1. システムの依存パッケージ
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. リポジトリをクローン
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. whisper.cpp をビルドし、モデルをダウンロード
make setup

# 4. バイナリをビルド：opons-voxd（デーモン本体）と opons-voxd-mcp（任意の MCP サーバー）
make

# 5. 実行
./opons-voxd
```

---

## ローカル AI のための GPU 解放（MCP サーバー）

Whisper のモデルは常に GPU メモリに常駐しています（`large-v3-turbo` と `medium` で約 3.8 GB）。ローカル LLM、画像生成モデル、学習ジョブなど、ほかの用途にその VRAM が必要になったときは、opons-voxd を終了せずにモデルをアンロードし、あとで再読み込みできます。アンロードするとプロセスの CUDA コンテキストも破棄されるため、opons-voxd は `nvidia-smi` の一覧から完全に消えます（VRAM 使用量 0 MB）。再読み込みにかかる時間は、モデルファイルがまだページキャッシュにあれば 1 秒未満、そうでなければ数秒です。

**トレイアイコンの状態**：

| アイコン | 状態 |
|:---:|---|
| 白いリング | モデル読み込み済み、待機中 |
| 赤い丸 | 録音中 |
| オレンジの丸 | 文字起こし中 |
| 斜線入りの灰色のリング（⊘） | モデルをアンロード済み、GPU メモリ解放済み、音声入力は無効 |
| 青い 4 分の 3 の円弧 | モデルを再読み込み中 |

アンロードされている間は、アイコンをクリックしてもプッシュ・トゥ・トークのホットキーを押しても録音は始まらず、エラー通知が表示されます。

**手動で行う場合**：トレイアイコンを右クリックして *Unload models (free GPU memory)*（モデルをアンロードして GPU メモリを解放）を選び、戻すときは *Reload models*（モデルを再読み込み）を選びます。

**AI エージェントから行う場合（MCP）**：デーモンを `-m` / `--mcp` 付きで起動します（デフォルトでは無効）。`make` を実行すると `opons-voxd-mcp` もビルドされます。これは stdio で通信する [Model Context Protocol](https://modelcontextprotocol.io) サーバーで、libc のみにリンクされています（RAM 使用量は約 1.5 MB）。提供するツールは `unload_models`、`reload_models`、`get_status` の 3 つです。Claude Code の場合は次のとおりです。

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

その他の多くのクライアント（Claude Desktop、Cursor、LM Studio など）では、次の JSON を使います。

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

詳しくは[メイン README](../README.md#free-the-gpu-for-local-ai-mcp-server)を参照してください。

---

## プッシュ・トゥ・トークのホットキー

同梱の `launch.sh` では、`ctrl+alt+w`（AZERTY 配列に推奨）または `ctrl+alt+z`（QWERTY 配列に推奨）を使っています。このキーは `A` の隣にあるため、タイピングの姿勢を崩さずに左手だけで組み合わせ全体を押せます（小指で `Ctrl`、親指で `Alt`、薬指か中指で `W`/`Z`）。また、ほとんどのデスクトップ環境ではデフォルトで何も割り当てられていません。設定は `OPONS_VOXD_PTT_HOTKEY` で行います。詳しくは[メイン README](../README.md#push-to-talk-hotkey)を参照してください。

---

## ライセンス

Whisper モデル（OpenAI）、whisper.cpp（Georgi Gerganov）、本プロジェクトのいずれも MIT ライセンスです。

完全なドキュメントは[メイン README](../README.md)を参照してください。

---

## コーディングスタイル

本プロジェクトは **Linux カーネルのコーディングスタイル**（`Documentation/process/coding-style.rst`）に、Olivier Pons による調整を加えたものに従っています。K&R スタイルの波括弧、4 スペースのインデント、typedef を使わない `struct name`、接頭辞なし（`s_`、`t_`、`e_`）、括弧なしの `return value;`、kernel-doc 形式のコメント、1 行は最大 80 文字です。詳しくは[メイン README](../README.md)を参照してください。
