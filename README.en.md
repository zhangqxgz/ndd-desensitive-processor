# Notepad-- (NDD) Desensitization Processor Plugin

[简体中文](README.md) | **English** | [日本語](README.ja.md)

> Detailed manual (Chinese): `2026-09-21-NDD脱敏处理器插件详细操作手册.md`
> (installation / usage / build / troubleshooting / internals)

A [Notepad--](https://gitee.com/cxasm/notepad--) (also known as **NDD**) plugin that
desensitizes and restores the current document in one click using a custom
keyword mapping table.

## Download

- Installer (recommended): [GitHub Releases](https://github.com/zhangqxgz/notepad--ndd-desensitize-plugin/releases) — download `desensitive_processor-v1.0.zip`
- Source: `git clone https://github.com/zhangqxgz/notepad--ndd-desensitize-plugin.git`

## Features

- Mapping table: enabled toggle, source text, replacement; add/remove/move rows
- Desensitize: replace every occurrence of the source text with the replacement
- Restore: reverse replacement back to the original text
- Single-pass scan with longest-match priority, so replacements never cascade
  (e.g. with `a→b` and `b→c`, the text `a b` becomes `b c`, not `c c`)
- Case-sensitive toggle (enabled by default)
- Selection-only toggle (disabled by default = whole document)
- Auto-generate 12-character random replacement strings
- Batch paste: `abc->aldjfakldfj`, `abc,xxx`, `abc<Tab>xxx`, etc.
- Import/export: JSON, CSV, plain text
- Ctrl+Z undoes the whole operation in a single step
- Mappings are saved automatically to `%APPDATA%\ndd-desensitive\mappings.json`

## Usage

1. Copy `desensitive_processor.dll` into the `plugin` folder of your NDD
   installation (or run `install-to-ndd.cmd` to install it into
   `C:\Program Files\Notepad--\plugin`)
2. Restart NDD and click **插件 (Plugins) → 脱敏处理器** in the menu bar
3. Configure mappings in the table, then click **脱敏 (Desensitize)** or
   **还原 (Restore)** to apply them to the current editor

Uninstall: run `uninstall-from-ndd.cmd`.

Diagnostic log: `%APPDATA%\ndd-desensitive\plugin.log`

## Distributing to another PC

1. Run `package.cmd` to build `dist\desensitive_processor-v1.0.zip`
2. Copy the zip to the target PC, extract it and run `install.cmd`
   (it locates NDD automatically and requests UAC elevation when needed;
   you can also copy the DLL into NDD's `plugin` folder manually)
3. Migrate mappings: use **Export** on the old PC and **Import** on the new one

Compatibility: Windows 64-bit, Qt5 builds of Notepad-- (official v3.x series,
tested on v3.8.2). If the target NDD folder contains `Qt6Core.dll` (Qt6 build),
the plugin must be rebuilt; the installer detects this and shows a warning.

## Related projects

- Notepad-- (NDD) main program: [Gitee](https://gitee.com/cxasm/notepad--) / [GitHub](https://github.com/cxasm/notepad--)
- Official NDD plugin project (examples & plugin submissions): [Gitee](https://gitee.com/cxasm/ndd-plugin) / [GitHub](https://github.com/cxasm/ndd-plugin)
- NDD community plugin organization: [Gitee ndd-community](https://gitee.com/ndd-community)
- This plugin on GitHub: https://github.com/zhangqxgz/notepad--ndd-desensitize-plugin

## License

GPL-3.0 (see `LICENSE`).
Notepad-- and QScintilla are both GPL-3.0, and this plugin links against
the editor library, so derivative works must be released under GPL-3.0.

## Building

### Requirements

- Qt 5.15.2 msvc2019_64 (`C:\Qt\5.15.2\msvc2019_64`)
- Visual Studio 2022 Build Tools (Desktop C++ workload)
- `sdk/` directory: NDD v3.8.2 plugin headers (`pluginGl.h`, `Qsci/*.h`) and `qmyedit_qt5.lib`

### Generate qmyedit_qt5.lib

This file must match the target NDD's `qmyedit_qt5.dll` exactly.
It is generated from the NDD installed on your machine:

```
tools\gen_qmyedit_lib.cmd
```

### Build and deploy to the portable test copy

```
build.cmd
```

Output: `build\out\desensitive_processor.dll`, automatically copied to
`test\Notepad--v3.8.2-win10-portable\plugin\`.

## Directory layout

```
src/                       Plugin source code
sdk/                       NDD plugin SDK (headers + import lib)
tools/gen_qmyedit_lib.cmd  Generate the import lib from the local NDD
tools/make_def.py          Convert dumpbin exports to a .def file
tools/engine_test/         Replacement engine unit tests (16 cases)
tools/load_test/           Plugin load smoke test
tools/scintilla_test/      QScintilla write-back behaviour test
build.cmd                  Build script
install-to-ndd.cmd         Install into the local NDD (admin)
uninstall-from-ndd.cmd     Uninstall from the local NDD
package.cmd                Build the distributable zip (dist\)
dist-template/             Distribution package template (install/uninstall/readme)
dist/                      Release package output
build/install_vs.cmd       VS Build Tools install script (one-time)
build/install_qt.cmd       Qt install script (one-time, CN mirrors)
test/                      Portable NDD test environment
```

## Implementation notes

- NDD's customized QScintilla has an unreliable internal selection flag in
  `replaceSelectedText()`. The plugin writes text back with the low-level
  messages `SCI_SELECTALL` + `SCI_REPLACESEL` (UTF-8 bytes), wrapped in
  `beginUndoAction/endUndoAction` so the operation undoes as one step
- Replacement uses a single-pass scan with longest-match priority,
  keeping desensitize/restore reversible and cascade-free

## Caveats

- The plugin is ABI-bound to NDD's Qt/compiler version: if NDD is upgraded to
  a Qt6 build, the plugin must be recompiled
- In the big-file fast mode the editor is read-only, so the plugin reports that
  the replacement cannot be performed
- Conflicting mappings (duplicate source or duplicate replacement) block the
  operation; a replacement that collides with another source text triggers a warning
- The local Qt 5.15.2 comes from a Qt online-repository archive whose
  `qconfig.pri` had `QT_EDITION=Enterprise`, changed to `OpenSource` for local
  builds only; for official distribution use an open-source Qt installed with
  the official online installer
