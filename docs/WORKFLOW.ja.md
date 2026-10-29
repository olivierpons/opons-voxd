# 私のワークフロー：母語で話し、英語で入力する

🌍 **翻訳**：
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

私の母語はフランス語ですが、ソフトウェアの仕事の大半は英語です。コミットメッセージ、コードコメント、GitHub の issue、海外のチームとのチャット、ドキュメント、どれも英語で書きます。流暢な英語をタイプすること自体は難しくありません。本当のボトルネックは、脳のほかの部分が母語で動いているあいだに英語で*考える*ことです。自分で翻訳しなくて済むなら、音声入力でこのボトルネックはすっかりなくなります。

## セットアップ

opons-voxd では、互いに独立した 2 つのプッシュ・トゥ・トークのホットキーを使っています。

- **`Ctrl+Alt+W`：通常の音声入力**。押したままフランス語で話して離すと、カーソル位置にフランス語のテキストが入力されます。メモやフランス語圏の同僚とのチャットなど、フランス語のまま残すものすべてに使います。
- **`Ctrl+Alt+K`：音声入力と翻訳**。操作は同じですが、フランス語で話すと、カーソル位置には*英語*が出てきます。コミットメッセージ、PR の説明、コードコメント、GitHub の issue、英語のチャットチャンネルなど、英語で読まれるものすべてに使います。

翻訳ツールへのコピー＆ペーストも、頭の切り替えも、打ち直しもありません。話せば、カーソルのある場所にそのまま英語が現れます。

## ホットキーが 1 つではなく 2 つある理由

翻訳をうまくこなせるのは、実際にそのタスク向けに学習したモデルだけです。日常のフランス語用ホットキーには、速さを優先して `large-v3-turbo` を使っています。ただ、このモデルは翻訳ではなく文字起こしの速度に合わせて最適化されているため、翻訳の品質は低めです。そこで翻訳用ホットキーでは、きちんと翻訳できる 2 つ目のモデル（`medium`）を読み込みます。2 つのモデルは同時に GPU メモリに常駐しますが、空きメモリが数 GB 以上ある GPU なら、この追加分は問題になりません。ローカル AI の処理のためにそのメモリを空けたいときは、その処理を動かしているエージェントが opons-voxd の MCP サーバー経由で両方のモデルをアンロードし、終わったら再読み込みします。その間はトレイアイコンが斜線入りの灰色のリングに変わるので、音声入力がオフだとわかります（[ローカル AI のための GPU 解放](README.ja.md#ローカル-ai-のための-gpu-解放mcp-サーバー)を参照）。

## 試してみる

これは opons-voxd の標準の設定で、フォークもパッチも要りません。`OPONS_VOXD_PTT_HOTKEY`、`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`、`OPONS_VOXD_TRANSLATE_MODEL`、`OPONS_VOXD_LANGUAGE` については、メイン README の [Configuration](../README.md#configuration) と [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) を参照してください。

ふだんは英語以外の言語で話し、仕事は英語でしているなら、おそらくこれがこのプロジェクトでいちばん役に立つ機能です。
