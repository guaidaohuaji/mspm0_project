# apfft_space

APFFT 算法实验的另一版本，结合 AD9959、ADC/DMA 与定时/串口处理。

## 核心内容

- `empty.c`：应用主逻辑与算法/外设调用。
- `empty.syscfg`：TI SysConfig 外设配置。
- `AD9959/`：DDS 驱动与频率/幅值/相位控制。
- `fft_my/`：FFT/APFFT 相关算法实现。
- CCS 工程配置：`.project`、`.cproject`、`.ccsproject`、`targetConfigs/`。

## 说明

该分支从原始 CCS 工程中整理，未提交 `Debug/`、clangd 缓存及编译产物。TI SDK 本体由本地 SDK/CCS 提供。
