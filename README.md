# ad9959_tuozhan_adc2

AD9959 DDS 扩展控制 + 双 ADC/DMA 采样实验，包含频率/幅度/相位配置接口。

## 核心内容

- `empty.c`：应用主逻辑与算法/外设调用。
- `empty.syscfg`：TI SysConfig 外设配置。
- `AD9959/`：DDS 驱动与频率/幅值/相位控制。
- CCS 工程配置：`.project`、`.cproject`、`.ccsproject`、`targetConfigs/`。

## 说明

该分支从原始 CCS 工程中整理，未提交 `Debug/`、clangd 缓存及编译产物。TI SDK 本体由本地 SDK/CCS 提供。
