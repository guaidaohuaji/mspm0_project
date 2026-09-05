# hardwareave

双 ADC/DMA 采样与硬件平均/采样链路验证工程，包含 UART 输出。

## 核心内容

- `empty.c`：应用主逻辑与算法/外设调用。
- `empty.syscfg`：TI SysConfig 外设配置。
- CCS 工程配置：`.project`、`.cproject`、`.ccsproject`、`targetConfigs/`。

## 说明

该分支从原始 CCS 工程中整理，未提交 `Debug/`、clangd 缓存及编译产物。TI SDK 本体由本地 SDK/CCS 提供。
