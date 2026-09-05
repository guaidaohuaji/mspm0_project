# THD

基于 ADC/DMA 采样与 1024 点 FFT 的总谐波失真（THD）计算实验工程。

## 平台

- MCU：TI MSPM0G3507
- IDE：Code Composer Studio
- 配置：TI SysConfig（`*.syscfg`）
- 工程配置：`.project` / `.cproject` / `.ccsproject`

## 主要源码

- `empty.c`

## 导入

在 Code Composer Studio 中选择 **Import CCS Projects**，指向本仓库根目录；如本机 MSPM0 SDK 版本不同，可通过 SysConfig 重新检查外设配置。

## 说明

该 branch 保留工程配置和源代码，已移除 `Debug/`、clangd 缓存、目标文件及链接输出，避免将 IDE/编译生成物提交到版本库。
