# OPA_PGA_MODE

**片上 OPA/PGA + ADC/DMA 采样实验**

基于 MSPM0G3507 配置片上 OPA/PGA 模拟前端，结合双 ADC/DMA、定时触发和 UART，对模拟链路与采样流程进行验证。

## 作品集保留内容

- `empty.c`：采样、DMA、中断及 OPA 增益配置主流程
- `empty.syscfg`：双 ADC、OPA、DMA、Timer、UART 等外设配置

已移除 `Debug/`、clangd 索引、目标文件和链接输出等生成内容。