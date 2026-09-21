# NDD マスキングプロセッサプラグイン (desensitive_processor)

[简体中文](README.md) | [English](README.en.md) | **日本語**

> 詳細マニュアル（中国語）：`2026-09-21-NDD脱敏处理器插件详细操作手册.md`
> （インストール / 使い方 / ビルド / トラブルシューティング / 内部実装）

[Notepad--](https://gitee.com/cxasm/notepad--)（NDD）用のプラグインです。
独自のキーワードマッピングテーブルを使い、現在のドキュメントを
ワンクリックでマスキング（脱敏）・復元できます。

## ダウンロード

- インストーラー（推奨）：[GitHub Releases](https://github.com/zhangqxgz/ndd-desensitive-processor/releases) から `desensitive_processor-v1.0.zip` をダウンロード
- ソースコード：`git clone https://github.com/zhangqxgz/ndd-desensitive-processor.git`

## 機能

- マッピングテーブル：有効／無効、元の文字列、置換後の文字列。行の追加・削除・並べ替えに対応
- マスキング：ドキュメント内の「元の文字列」をすべて「置換後の文字列」に置換
- 復元：マッピングに従って逆方向に置換し、元の文字列に戻す
- シングルパス走査＋最長一致優先のため、置換結果が再置換されない
  （例：`a→b`、`b→c` があっても `a b` は `b c` になり、`c c` にはならない）
- 大文字小文字を区別するスイッチ（デフォルト：オン）
- 選択範囲のみ処理するスイッチ（デフォルト：オフ＝ドキュメント全体）
- 12 文字のランダム置換文字列を自動生成
- 一括貼り付け：`abc->aldjfakldfj`、`abc,xxx`、`abc<Tab>xxx` などの形式に対応
- インポート／エクスポート：JSON、CSV、テキスト
- Ctrl+Z で操作全体を 1 ステップで元に戻せる
- マッピング設定は `%APPDATA%\ndd-desensitive\mappings.json` に自動保存

## 使い方

1. `desensitive_processor.dll` を NDD インストールディレクトリの `plugin`
   フォルダにコピーします（`install-to-ndd.cmd` を実行すると
   `C:\Program Files\Notepad--\plugin` に自動インストールできます）
2. NDD を再起動し、メニューバーの **插件（プラグイン）→ 脱敏处理器** をクリックします
3. テーブルにマッピングを設定し、**脱敏（マスキング）** または
   **还原（復元）** をクリックすると、現在のエディタに反映されます

アンインストール：`uninstall-from-ndd.cmd` を実行します。

診断ログ：`%APPDATA%\ndd-desensitive\plugin.log`

## 他の PC への配布

1. `package.cmd` を実行して `dist\desensitive_processor-v1.0.zip` を作成します
2. 対象 PC に zip をコピーして解凍し、`install.cmd` を実行します
   （NDD を自動検出し、必要な場合は UAC の昇格を要求します。
   DLL を NDD の `plugin` フォルダに手動でコピーしても構いません）
3. マッピングの移行：旧 PC で **エクスポート**、新 PC で **インポート**

対応環境：Windows 64 ビット版、Qt5 ビルドの Notepad--（公式 v3.x 系、v3.8.2 で動作確認済み）。
対象の NDD フォルダに `Qt6Core.dll`（Qt6 ビルド）がある場合、プラグインは
再コンパイルが必要です。インストーラーが検出して警告を表示します。

## ライセンス

GPL-3.0（`LICENSE` を参照）。
Notepad-- と QScintilla はいずれも GPL-3.0 であり、本プラグインは
エディタライブラリにリンクしているため、派生作品も GPL-3.0 での公開が必要です。

## ビルド

### 必要条件

- Qt 5.15.2 msvc2019_64（`C:\Qt\5.15.2\msvc2019_64`）
- Visual Studio 2022 Build Tools（Desktop C++ ワークロード）
- `sdk/` ディレクトリ：NDD v3.8.2 のプラグインヘッダー（`pluginGl.h`、`Qsci/*.h`）と `qmyedit_qt5.lib`

### qmyedit_qt5.lib の生成

このファイルは対象 NDD の `qmyedit_qt5.dll` と完全に一致している必要があります。
インストール済みの NDD から次のスクリプトで生成します：

```
tools\gen_qmyedit_lib.cmd
```

### ビルドとポータブル版への配備

```
build.cmd
```

成果物：`build\out\desensitive_processor.dll`（
`test\Notepad--v3.8.2-win10-portable\plugin\` へ自動コピーされます）。

## ディレクトリ構成

```
src/                       プラグインのソースコード
sdk/                       NDD プラグイン SDK（ヘッダー + import lib）
tools/gen_qmyedit_lib.cmd  ローカル NDD から import lib を生成
tools/make_def.py          dumpbin のエクスポート表を .def に変換
tools/engine_test/         置換エンジンの単体テスト（16 項）
tools/load_test/           プラグイン読み込みスモークテスト
tools/scintilla_test/      QScintilla 書き戻し動作の検証
build.cmd                  ビルドスクリプト
install-to-ndd.cmd         ローカル NDD へインストール（管理者）
uninstall-from-ndd.cmd     ローカル NDD からアンインストール
package.cmd                配布用 zip の作成（dist\）
dist-template/             配布パッケージのテンプレート（インストール/アンインストール/説明）
dist/                      リリースパッケージの出力先
build/install_vs.cmd       VS Build Tools インストールスクリプト（初回のみ）
build/install_qt.cmd       Qt インストールスクリプト（初回のみ、中国ミラー）
test/                      ポータブル版 NDD のテスト環境
```

## 実装上のポイント

- NDD カスタム版 QScintilla の `replaceSelectedText()` は内部の選択状態フラグが
  信頼できないため、書き戻しには低レベルメッセージ
  `SCI_SELECTALL` + `SCI_REPLACESEL`（UTF-8 バイト）を使用し、
  `beginUndoAction/endUndoAction` で囲んで 1 回の Undo にまとめています
- 置換はシングルパス走査＋最長一致優先で、マスキング／復元の可逆性を保証し、
  連鎖置換を防ぎます

## 注意事項

- 本プラグインは NDD の Qt／コンパイラ ABI に依存します。NDD が Qt6 ビルドに
  アップグレードされた場合は再コンパイルが必要です
- 大きいファイルの高速モードではエディタが読み取り専用のため、
  置換できない旨のメッセージを表示します
- マッピングの競合（元の文字列の重複、置換後の文字列の重複）は実行を中断します。
  置換後の文字列が他のマッピングの元の文字列と衝突する場合は警告を表示します
- ローカルの Qt 5.15.2 は Qt オンラインリポジトリのアーカイブ由来で、
  `qconfig.pri` の `QT_EDITION=Enterprise` をローカルビルド用に `OpenSource` へ
  変更しています。正式配布には公式オンラインインストーラーで導入した
  オープンソース版 Qt の使用を推奨します
