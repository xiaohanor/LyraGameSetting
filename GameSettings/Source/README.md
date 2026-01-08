## GameSettings 蓝图接口速览

本插件主要在 C++ 中使用，但以下节点可直接在蓝图里搭建设置页面或快速原型：

- **绑定数据源（等同于 C++ 宏 GET_*_FUNCTION_PATH）**  
  - `SetDynamicGetterPath` / `SetDynamicSetterPath`：传入字符串数组作为反射链路，例如 `["GetLocalSettings", "GetFullscreenMode"]`。
- **离散选项（UGameSettingValueDiscreteDynamic 及子类）**  
  - `AddDynamicOption` / `RemoveDynamicOption`、`SetDefaultValueFromString`、`GetOptionValues`、`HasDynamicOption`、`GetValueAsString`/`SetValueFromString`。  
  - Bool 版本还可通过 `SetTrueText` / `SetFalseText` 自定义显示文本。
- **标量选项（UGameSettingValueScalarDynamic）**  
  - `SetDisplayFormatPreset` 快速选择常用显示格式（整数、百分比、保留 1/2 位小数等）。  
  - `SetSourceRangeAndStepValues`、`SetMinimumLimitValue`/`ClearMinimumLimit`、`SetMaximumLimitValue`/`ClearMaximumLimit` 控制滑条范围与边界。

> 提示：DevName、DisplayName 等基础字段依旧沿用 `UGameSetting` 的 BlueprintCallable 接口；完成配置后可像 C++ 版本一样调用 `Initialize`、`RefreshEditableState`、`Apply` 等生命周期函数。
