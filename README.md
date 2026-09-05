# LCD_module

MSPM0G3507 LCD 驱动与波形绘制模块验证工程，包含中文/字符显示、图形与波形显示接口。

## 核心内容

- `empty.c`：应用主逻辑与算法/外设调用。
- `empty.syscfg`：TI SysConfig 外设配置。
- `LCD/`：LCD 驱动、字体/图片与绘图代码。
- CCS 工程配置：`.project`、`.cproject`、`.ccsproject`、`targetConfigs/`。

## 说明

该分支从原始 CCS 工程中整理，未提交 `Debug/`、clangd 缓存及编译产物。TI SDK 本体由本地 SDK/CCS 提供。
