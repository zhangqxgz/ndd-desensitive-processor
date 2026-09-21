# NDD 脱敏处理器插件（desensitive_processor）

> 详细操作手册：`2026-09-21-NDD脱敏处理器插件详细操作手册.md`（同目录，含安装/使用/编译/排障/原理）

Notepad--（NDD）插件：按自定义「关键词映射表」对当前文档执行一键脱敏/还原。

## 下载

- 安装包（推荐）：[GitHub Releases](https://github.com/zhangqxgz/ndd-desensitive-processor/releases) 下载 `desensitive_processor-v1.0.zip`
- 源码：`git clone https://github.com/zhangqxgz/ndd-desensitive-processor.git`

## 功能

- 映射表：启用开关、原文、替换为，可增删、上移/下移
- 脱敏：把文档中所有「原文」替换为「替换为」
- 还原：按映射反向替换回原文
- 单遍扫描 + 最长匹配优先，避免替换结果被二次替换（如 a→b、b→c 不会连锁）
- 区分大小写开关（默认开启）
- 仅处理选中文本开关（默认关闭，即全文）
- 自动生成 12 位随机替换串
- 批量粘贴：`abc->aldjfakldfj`、`abc,xxx`、`abc<Tab>xxx` 等格式
- 导入/导出：JSON、CSV、文本
- 支持 Ctrl+Z 一步撤销本次操作
- 映射配置自动保存到 `%APPDATA%\ndd-desensitive\mappings.json`

## 使用

1. 把 `desensitive_processor.dll` 复制到 NDD 安装目录的 `plugin` 文件夹
   （或双击 `install-to-ndd.cmd` 一键安装到 `C:\Program Files\Notepad--\plugin`）
2. 重启 NDD，在「插件」菜单点击「脱敏处理器」
3. 在表格中配置映射，点击「脱敏」或「还原」对当前编辑框生效

卸载：双击 `uninstall-from-ndd.cmd`。

诊断日志：`%APPDATA%\ndd-desensitive\plugin.log`

## 分发到其他电脑

1. 运行 `package.cmd` 生成发布包 `dist\desensitive_processor-v1.0.zip`
2. 把 zip 拷到目标电脑解压，双击 `install.cmd`（自动查找 NDD 目录，
   必要时弹 UAC；也可选择手动把 DLL 放进 NDD 的 `plugin` 目录）
3. 映射迁移：在插件窗口用「导出」，到新电脑用「导入」

兼容性：适用于 Windows 64 位 Qt5 版 Notepad--（官方 v3.x 系列）。
若目标 NDD 目录内有 `Qt6Core.dll`（Qt6 版本），插件需重新编译，
安装脚本会检测并给出提示。

## 开源协议

本项目采用 GPL-3.0 协议（见 `LICENSE`）。
原因：Notepad-- 与 QScintilla 均为 GPL-3.0，本插件链接其编辑器库，
衍生作品需以 GPL-3.0 发布。

## 编译

### 依赖

- Qt 5.15.2 msvc2019_64（`C:\Qt\5.15.2\msvc2019_64`）
- Visual Studio 2022 Build Tools（Desktop C++ 工作负载）
- `sdk/` 目录：NDD v3.8.2 的插件头文件（`pluginGl.h`、`Qsci/*.h`）与 `qmyedit_qt5.lib`

### 生成 qmyedit_qt5.lib

该文件需与目标 NDD 的 `qmyedit_qt5.dll` 严格匹配，由脚本从已安装的 NDD 生成：

```
tools\gen_qmyedit_lib.cmd
```

### 编译并复制到测试版

```
build.cmd
```

产物：`build\out\desensitive_processor.dll`，会自动复制到
`test\Notepad--v3.8.2-win10-portable\plugin\`。

## 目录说明

```
src/                       插件源码
sdk/                       NDD 插件 SDK（头文件 + import lib）
tools/gen_qmyedit_lib.cmd  从本机 NDD 生成 import lib
tools/make_def.py          dumpbin 导出表转 .def
tools/engine_test/         替换引擎单元测试（16 项）
tools/load_test/           插件加载烟雾测试
tools/scintilla_test/      QScintilla 写回机制验证
build.cmd                  编译脚本
install-to-ndd.cmd         安装到本机正式版 NDD（管理员）
uninstall-from-ndd.cmd     从本机正式版 NDD 卸载
package.cmd                生成可分发 zip（dist\）
dist-template/             分发包模板（安装/卸载脚本、说明）
dist/                      发布包输出目录
build/install_vs.cmd       VS Build Tools 安装脚本（一次性）
build/install_qt.cmd       Qt 安装脚本（一次性，清华/USTC 镜像）
test/                      绿色版 NDD 测试环境
```

## 实现说明

- NDD 定制版 QScintilla 的 `replaceSelectedText()` 内部选中标志不可靠，
  实际写回使用底层消息 `SCI_SELECTALL` + `SCI_REPLACESEL`（UTF-8 字节），
  并用 `beginUndoAction/endUndoAction` 包成一个撤销步骤
- 替换采用单遍扫描 + 最长匹配优先，保证脱敏/还原可逆、不级联

## 注意

- 插件与 NDD 的 Qt / 编译器 ABI 绑定：NDD 若升级到 Qt6 版本，插件需重新编译
- 大文件快速模式下编辑器是只读的，插件会提示无法替换
- 映射冲突（原文重复、替换文重复）会阻止执行；替换文与其它原文冲突会给出提醒
- 本机 Qt 5.15.2 来自 Qt 在线仓库归档包，qconfig.pri 中 `QT_EDITION=Enterprise`
  已被改为 `OpenSource`（仅为本地构建；正式分发建议使用官方在线安装器安装的开源 Qt）

