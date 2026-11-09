# Memories.md - QuarkMeta 全局设计理念与历史记忆真理源 (SSOT)

## 1. 矢量图标库管理理念与规范 (Vector SVG Icons Specification)
- **100% 纯矢量图标真理源**：`QuarkMeta_Source/src/ui/SvgIcons.h` 中的 `SvgIcons::icons` 映射表为全软件矢量图标的唯一 SSOT。
- **键名规范与防覆盖铁律**：新增加的独立图标键名采用对应的 SVG 文件完整名称（如 `"Cross-Potent-1.svg"` 等）。**若遇到既有键名重复冲突，必须严格采用递增序号方式（例如 `link-1.svg`、`link-2.svg`、`link-3.svg`、`link-N.svg`）进行区分，决不可发生任何覆盖或篡改既有键名行为**。对于内容 100% 相同且已存在的图标（如 `Monitoring-1.svg`），原样保留即可。
- **动态调色规范**：所有嵌入 `SvgIcons::icons` 的 SVG 矢量代码必须在根 `<svg>` 节点配置 `fill="currentColor"`（或在对应描边与填充中使用 `currentColor`），禁止硬编码固定单色，使应用层通过 `SvgIconRenderer` / `UiHelper` 根据主题动态渲染。
- **资源来源与映射一致性**：`QuarkMeta_Source/resources/09-11-2026 1127/` 中的 15 个图标文件与 `SvgIcons::icons` 保持严格映射，新扩展图标统一追加收敛至 SSOT 表。

## 2. 交互设计与 UI 规范
- **ToolTipOverlay 规则**：任何引用 `ToolTipOverlay` 的场景，绝不可以追随鼠标位置显示 ToolTipOverlay。
- **样式规范真理源**：UI 界面样式、主题管理、控件外观等重构或新增修改必须遵守 `Memories.md`，严禁在 C++ 代码中内联 `setStyleSheet` 硬编码样式，统一通过 `resources/style.qss` 与 `ThemeManager` 实施。
- **重构数值零篡改**：代码重构与解耦过程中，既有数值参数（颜色、尺寸、边距、圆角等）必须 100% 原样照抄。
