# Lyra 游戏设置系统（GameSettings 插件 + Settings 业务层）速查指南

本指南面向虚幻引擎初学者，帮助你快速理解 Lyra 样例项目中“游戏设置”体系的两层结构：

- **通用插件层（GameSettings）**：位于 `GameSettings/Source`，提供可复用的设置框架（`UGameSetting`、数据源、可编辑状态、筛选、注册表、UI 控件等），与具体业务无关。
- **业务实现层（Settings）**：位于 `Settings/`，依托插件实现 Lyra 的具体设置项、持久化、分类页面和默认值，文件以 `LyraGameSettingRegistry_*` 和 `LyraSettings*` 为主。

> 如果只想快速添加一个新选项，直接跳到「五步添加一个新设置」章节。

---

## 目录与关键文件

- **插件核心（可复用）**
  - `GameSetting.h` / `GameSettingValue*.h`：基类与离散/标量/动态枚举值、动作、集合等具体类型。
  - `GameSettingRegistry.h`：注册表，负责初始化/注册/筛选所有设置、派发事件、保存等。
  - `DataSource/`：动态数据源，允许设置通过反射链路调用 Getter/Setter（见 Lyra 中的宏）。
  - `EditCondition/`：编辑条件，用于控制可见/可编辑状态（平台特性、主副玩家、其他设置依赖等）。
  - `Widgets/`：内置的列表、详情面板等 UI 组件，消费 `UGameSetting` 数据。

- **Lyra 业务层**
  - `LyraSettingsLocal.*`：继承 `UGameUserSettings`，存放**本机**相关数据（显示、帧率、音频设备、性能统计等），Config 驱动。
  - `LyraSettingsShared.*`：继承 `ULocalPlayerSaveGame`，存放**账号/云端可共享**数据（按键反转、色盲模式、手柄震动等），SaveGame 驱动。
  - `LyraGameSettingRegistry.*`：注册表入口，按类别创建页面并调用 `RegisterSetting`。
    - `LyraGameSettingRegistry_Video.cpp`、`_Audio.cpp`、`_Gameplay.cpp`、`_MouseAndKeyboard.cpp`、`_Gamepad.cpp`、`_PerfStats.cpp`：各分类的具体设置定义。
    - 宏 `GET_SHARED_SETTINGS_FUNCTION_PATH` / `GET_LOCAL_SETTINGS_FUNCTION_PATH` 将 Getter/Setter 链接到对应数据源。
  - `CustomSettings/`：对通用值类型的项目化扩展（如分辨率、整体画质、移动端 FPS 选项等）。

---

## 系统运行流程（从 UI 到持久化）

1. **注册表创建**：`ULyraGameSettingRegistry::Get(LocalPlayer)`（见 `LyraGameSettingRegistry.cpp`）为本地玩家构建一个注册表实例。
2. **分类初始化**：`OnInitialize` 依次调用各 `_Video/_Audio/...` 初始化函数，返回 `UGameSettingCollection`，并通过 `RegisterSetting` 递归注册。
3. **数据绑定**：每个设置通过 `SetDynamicGetter/Setter` 绑定到 Local/Shared 设置对象上的函数；值变化会触发 `OnSettingChangedEvent`。
4. **编辑条件**：通过 `AddEditCondition` / `AddEditDependency` 控制可用性（平台特性、其他设置状态等），并在 UI 刷新时计算 `EditableState`。
5. **应用与保存**：用户修改后调用 `SaveChanges()`：
   - 本地设置：`LyraLocalSettings->ApplySettings(false)`（处理分辨率/帧率等）。
   - 共享设置：`LyraSharedSettings->ApplySettings()` 然后 `SaveSettings()`（SaveGame 持久化）。

---

## 重要概念速览

- **UGameSetting & 派生类型**
  - `UGameSettingValueDiscrete` / `...Dynamic_Enum/Number`：多选或数字刻度。
  - `UGameSettingValueScalar`：连续区间值。
  - `UGameSettingAction`：执行一次性动作（如打开安全区域编辑器）。
  - `UGameSettingCollection`：可嵌套的分组/页面。
- **数据源（DataSourceDynamic）**：通过字符串链路调用 Getter/Setter，避免手写绑定；Lyra 用宏生成链路。
- **编辑条件（EditCondition）**：`FWhenPlatformHasTrait`、`FWhenCondition` 等控制可见性/可编辑性，支持依赖其他设置。
- **筛选与事件**：`GameSettingFilterState` 允许按标签/输入设备筛选；`OnSettingChanged/OnSettingApplied/OnSettingNamedAction` 事件帮助 UI 或逻辑响应。

---

## 五步添加一个新设置（面向初学者）

1. **选存储层**  
   - 设备相关或无需云同步 → `LyraSettingsLocal`（Config）。  
   - 玩家账号相关或需要跨设备同步 → `LyraSettingsShared`（SaveGame）。

2. **在设置类中添加字段和访问器**  
   - 使用 `ChangeValueAndDirty`（Shared）或等效逻辑确保脏标记与事件触发。  
   - 为需要立即生效的值调用应用函数（如 `ApplyInputSensitivity()`、`ApplyDisplayGamma()`）。

3. **在对应分类的 Registry 文件里创建设置项**  
   - 选择合适的值类型（离散枚举、数字、标量或动作）。  
   - 设置 DevName/DisplayName/Description，必要时设置默认值。  
   - 通过 `SetDynamicGetter/Setter(GET_LOCAL/SHARED_SETTINGS_FUNCTION_PATH(...))` 绑定数据源。  
   - 使用 `AddEditCondition`/`AddEditDependency` 控制可用性与刷新逻辑。

4. **注册与分组**  
   - 将新设置放入合适的 `UGameSettingCollection`，再确保父集合被 `RegisterSetting` 链接到注册表（参考现有 `_Video/_Audio` 初始化函数）。

5. **验证与保存**  
   - 在编辑器运行游戏，打开设置 UI 验证显示/可用性。  
   - 触发 `SaveChanges()`，确认 Local 设置已 `ApplySettings`、Shared 设置已保存（可在下次启动复现）。

---

## 现有分类与示例（可作为模板）

- **Video (`LyraGameSettingRegistry_Video.cpp`)**
  - 窗口模式、分辨率、移动端 FPS、画质、色盲模式与强度、亮度/安全区域动作。
  - 演示：`AddEditDependency` 让分辨率依赖窗口模式；`FWhenPlatformHasTrait` 处理平台特性。
- **Audio (`_Audio.cpp`)**
  - 总音量/音乐/音效/语音/设备选择、后台播放、耳机模式、HDR 音频等。
- **Gameplay (`_Gameplay.cpp`)**
  - 字幕开关/样式、延迟指示、性能 HUD 开关等。
- **Mouse & Keyboard (`_MouseAndKeyboard.cpp`) / Gamepad (`_Gamepad.cpp`)**
  - 鼠标灵敏度、ADS 乘数、轴反转、手柄震动/触发反馈、死区、预设灵敏度等。
- **Performance Stats (`_PerfStats.cpp`)**
  - 可显示性能指标的启用/模式选择。

查找示例：例如视频色盲模式设置使用
`SetDynamicGetter(GET_SHARED_SETTINGS_FUNCTION_PATH(GetColorBlindMode));`
并设定默认值 `GetDefault<ULyraSettingsShared>()->GetColorBlindMode()`。

---

## 调试与排查小贴士

- 注册表在 `IsFinishedInitializing()` 返回真之前不会暴露设置，确保 Shared 设置已加载完成（见 `LyraGameSettingRegistry::IsFinishedInitializing`）。
- 修改依赖项后调用 `RefreshEditableState` 或确保设置之间通过 `AddEditDependency` 自动联动。
- 本地设置调用 `ApplySettings(false)` 会触发分辨率切换等需要立即生效的逻辑；确认在保存时已被调用。
- 需要 UI 跳转时使用 `OnExecuteNavigationEvent`；需要自定义动作时使用 `UGameSettingAction`。
- 文本内容要本地化：使用 `LOCTEXT` 提供显示名/描述/提示。

---

## 建议的学习路径

1. 先读 `GameSettings/Source/Public/GameSetting*.h` 了解基础类型与生命周期（`Initialize` → `Startup` → `Apply`）。
2. 对照 `LyraGameSettingRegistry_Video.cpp` 阅读一个完整分类的构建方式。
3. 修改一个低风险设置（如字幕颜色）并在编辑器 UI 中验证保存/加载。
4. 按「五步添加」流程添加你自己的设置，必要时在 `CustomSettings/` 中创建派生类型。

掌握以上内容后，你就可以在 Lyra 或其他项目中自信地扩展这套设置系统。祝学习顺利！*** End Patch"});?>
