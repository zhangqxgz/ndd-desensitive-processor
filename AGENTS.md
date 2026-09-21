# NDD 脱敏处理器插件 - 项目记忆

> 详细手册：`2026-09-21-NDD脱敏处理器插件详细操作手册.md`（同目录）
> 全局记忆：`/home/qx/.config/opencode/AGENTS.md`、`C:\dev\code\AGENTS.md`

## 项目信息

- 目录：`C:\dev\code\ndd_plugins\desensitive_processor`（WSL：`/mnt/c/dev/code/ndd_plugins/desensitive_processor`）
- 用途：Notepad--（NDD）插件，按关键词映射表对当前文档一键脱敏/还原
- 当前版本：v1.0（2026-09-21）
- 发布包：`dist\desensitive_processor-v1.0.zip`

## 关键环境（本机已装好）

- Qt 5.15.2 msvc2019_64：`C:\Qt\5.15.2\msvc2019_64`
  - 由 qtbase 在线归档包手动解压（USTC 镜像，sha256 e563de40...）
  - `mkspecs\qconfig.pri` 的 `QT_EDITION` 已由 Enterprise 改为 OpenSource（备份 .enterprise.bak）
- VS 2022 Build Tools：`C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools`（MSVC 14.44/v143，未装 v142）
- NDD 正式版：`C:\Program Files\Notepad--`（v3.8.2，Qt 5.15.2 x64）
- NDD 绿色测试版：`test\Notepad--v3.8.2-win10-portable\`
- 插件 SDK：`sdk\`（从 NDD v3.8.2 源码提取的 pluginGl.h + Qsci 头文件；`sdk\lib\qmyedit_qt5.lib` 由 tools\gen_qmyedit_lib.cmd 从本机 NDD 的 qmyedit_qt5.dll 生成，2403 个导出符号）

## 常用命令

- `build.cmd`：编译并自动部署到测试版 NDD
- `tools\engine_test\build_run.cmd`：引擎单测（16 项，应全 PASS）
- `package.cmd`：生成发布 zip
- `install-to-ndd.cmd` / `uninstall-from-ndd.cmd`：安装/卸载到正式版 NDD
- `tools\gen_qmyedit_lib.cmd`：换 NDD 版本时重新生成 import lib

## 技术要点（踩坑记录）

1. NDD 插件 = 导出 `NDD_PROC_IDENTIFY` + `NDD_PROC_MAIN` 的 DLL，放 `<NDD>\plugin\`；接口签名为
   `std::function<QsciScintilla*(QWidget*)>` 版本（v3.8.2 与 master 一致）
2. **NDD 定制版 QScintilla 的 `replaceSelectedText()` 无效、`setText()` 会清空文档**：
   写回必须用底层消息 `SCI_SELECTALL` + `SCI_REPLACESEL`（UTF-8 字节），
   包在 `beginUndoAction/endUndoAction` 里；详见手册 5.3/5.4
3. 替换引擎：单遍扫描 + 最长匹配优先（正则 alternation 按 key 长度降序），保证不级联、正反向可逆
4. 插件配置：`%APPDATA%\ndd-desensitive\mappings.json`；日志：`%APPDATA%\ndd-desensitive\plugin.log`
5. 每次点击菜单 NDD 都会重新调 `NDD_PROC_MAIN`，插件需用静态实例避免重复开窗
6. Qt6 版 NDD 不兼容本插件，需重编译
7. aqtinstall 装 Qt 5.15.2 在 USTC 镜像会找不到 XML；清华镜像 qtbase 包损坏；
   最终采用「手动下载 qtbase 归档 + 7z 解压 + qconfig.pri 改 OpenSource」方案

## 约定

- 新增/修改手册一律使用「日期+中文内容」文件名，如 `2026-09-21-NDD脱敏处理器插件详细操作手册.md`
