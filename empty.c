
#include "ti_msp_dl_config.h"
#include "AD9959.h"
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "arm_math.h"
#include "arm_const_structs.h"

#define FFT_LENGTH      1024				// FFT长度，默认是1024点FFT
volatile uint16_t mode=0;
volatile uint8_t length=0;
uint16_t ADC0_Value[FFT_LENGTH],ADC1_Value[FFT_LENGTH];
volatile bool ADC0FLAG,ADC1FLAG;

int fputc(int ch, FILE *f)
 {
        DL_UART_transmitData(UART_1_INST, (uint8_t)ch);//发送数据
        while(DL_UART_isBusy(UART_1_INST));//等待UART处于非BUSY状态
    return (ch);
 }
 int fputs(const char *_ptr, register FILE *_fp)
 {
 unsigned int i, len;
 len = strlen(_ptr);
        for(i=0 ; i<len ; i++)
        DL_UART_transmitData(UART_1_INST, (uint8_t)_ptr[i]);//发送数据
    return len;
 }


void ad9959_toset(uint8_t Channel, uint32_t Freq, uint16_t Ampli,  uint16_t Phase)
{
    // 设置通道 CH0 的输出频率
    AD9959_Set_Fre(Channel, Freq);
    // 设置通道 CH0 的输出幅度为最大值（1023）   1023对应500mVpp(实测并不一定， 有偏差)
    AD9959_Set_Amp(Channel, Ampli);
    // 设置通道 CH0 的输出相位为 0 度
    AD9959_Set_Phase(Channel, Phase);
    // 更新数据到 DDS 模块
   
}

int main(void)
{
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART1_INT_IRQn);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);//设置ADC对应的内存地址
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);//设置搬运终点
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);//设置搬运长度
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);//设置ADC对应的内存地址
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);//设置搬运终点
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);//设置搬运长度
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);//使能DMA通道
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);//使能DMA通道
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);//使能中断
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);//使能中断
    ADC0FLAG=false;
    ADC1FLAG=false;
    AD9959_Init();
    ad9959_toset(AD9959_CH1, 100000, 1023 ,  0);
    ad9959_toset(AD9959_CH0, 100000, 1023 ,  90);//90-》50
    IO_Update();
    
    while (1) {
      if(ADC1FLAG==true){
        ADC0FLAG=false;
        ADC1FLAG=false;
       // DL_Timer_setLoadValue(TIMA0, (uint32_t)32000000/samplefre-1);//////////////0.3887              0.1181
        DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);//设置ADC对应的内存地址
        DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);//设置搬运终点
        DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);//设置搬运长度
        DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);//设置ADC对应的内存地址
        DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);//设置搬运终点
        DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);//设置搬运长度
        DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);//使能DMA通道
        DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);//使能DMA通道
      }
    }
}
void ADC12_0_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            ADC0FLAG = true;
            break;
        default:
            break;
    }
 }
 void ADC12_1_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            ADC1FLAG = true;
            break;
        default:
            break;
    }
 }
 void UART_1_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_1_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            mode = DL_UART_Main_receiveData(UART_1_INST);
            break;

        default://其他的串口中断
            break;
    }
    
}