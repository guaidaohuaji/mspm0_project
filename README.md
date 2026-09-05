# MSPM0G3507 工程合集

该仓库用于整理多个 **TI MSPM0G3507 / Code Composer Studio** 实验工程。为了避免多个互不相关的工程混在同一目录中，**每个工程放在独立 Git branch**；`main` 分支仅作为项目索引。

## 工程索引

| Branch | 简介 |
|---|---|
| `A_2020_ping` | 双 ADC + DMA 采样、1024 点 FFT 与频谱分析实验，包含 UART 输出和信号处理流程。 |
| `A_2020model` | 双 ADC + DMA 采样与 1024 点 FFT 的模型验证工程，用于频域特征/幅值处理实验。 |
| `C2023final` | AD9959 DDS 控制与双 ADC/DMA 采样结合的综合实验工程，包含 FFT 相关处理和 UART 调试。 |
| `LCD_module` | MSPM0G3507 LCD 驱动与波形绘制模块验证工程，包含中文/字符显示、图形与波形显示接口。 |
| `OPA_PGA_MODE` | 片上 OPA/PGA 配置、ADC/DMA 采样和频域分析的模拟前端验证工程。 |
| `THD` | 基于 ADC/DMA 采样与 1024 点 FFT 的总谐波失真（THD）计算实验工程。 |
| `ad9959_tuozhan_adc2` | AD9959 DDS 扩展控制 + 双 ADC/DMA 采样实验，包含频率/幅度/相位配置接口。 |
| `apfft_nospace` | 基于 512 点数据的 APFFT（全相位 FFT）算法实验，结合 AD9959、ADC/DMA 与 UART。 |
| `apfft_space` | APFFT 算法实验的另一版本，结合 AD9959、ADC/DMA 与定时/串口处理。 |
| `duoji_pwm` | PWM/舵机控制实验，结合 ADC/DMA、AD9959 和 FFT 相关代码进行综合验证。 |
| `hardwareave` | 双 ADC/DMA 采样与硬件平均/采样链路验证工程，包含 UART 输出。 |
| `lcd_my` | LCD 驱动、字符/中文显示和波形绘制实验工程，包含正弦波数据生成与显示。 |

## 浏览方式

```bash
git clone <repository-url>
cd mspm0_project
git branch -a
git switch THD
```

每个 branch 对应一个原始实验工程的**求职作品集版本**，重点保留 `empty.c`、`empty.syscfg` 以及部分必要的工程/模块文件，方便查看主程序、外设配置和算法流程。

## 整理说明

- 已移除 `Debug/`、`.clangd` 索引、目标文件、链接输出等编译生成内容。
- 不重复收录 TI SDK / CMSIS 等可由官方 SDK 提供的通用代码。
- 个别分支为了控制作品集体积，仅展示核心源码与 SysConfig，因此不承诺 clone 后可直接编译；如需复现实验，应使用对应 `empty.syscfg` 中记录的 MSPM0 SDK / SysConfig 版本补齐依赖。
- 分支中的功能说明依据原始源码和 SysConfig 配置整理，未修改原项目的核心实现逻辑。
