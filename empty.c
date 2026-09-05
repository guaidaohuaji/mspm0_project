
#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "arm_math.h"
#include "arm_const_structs.h"

#define LENGTH      512
volatile uint16_t mode=0,index_1=0,index_2=0;
volatile uint8_t length=0;
uint16_t ADC0_Value[LENGTH*2],ADC1_Value[LENGTH*2];
volatile bool ADC0FLAG,ADC1FLAG;
volatile uint16_t vpp1,vpp2;
volatile float Phase_1=0,Phase_2=0,Phase=0,fmax_1=0,fmax_2=0,votalge=0,realvo=0;
uint16_t max_1=0,min_1=4096;

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



int main(void)
{
    float sum=0;
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART1_INT_IRQn);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);//设置ADC对应的内存地址
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);//设置搬运终点
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, LENGTH*2);//设置搬运长度
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);//设置ADC对应的内存地址
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);//设置搬运终点
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, LENGTH*2);//设置搬运长度
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);//使能DMA通道
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);//使能DMA通道
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);//使能中断
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);//使能中断
    ADC0FLAG=false;
    ADC1FLAG=false;
    DL_TimerA_startCounter(TIMER_0_INST);
    while (1) {
      if(ADC1FLAG==true){
        ADC0FLAG=false;
        ADC1FLAG=false;
        sum=0;
        for(int i=0;i<1024;i++){
            sum+=ADC0_Value[i];
        }
        sum/=1024;
        votalge=sum*3300.0/4096+1;
        realvo=0.9973*votalge+2.2814;
        DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);//设置ADC对应的内存地址
        DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);//设置搬运终点
        DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, LENGTH*2);//设置搬运长度
        DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);//设置ADC对应的内存地址
        DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);//设置搬运终点
        DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, LENGTH*2);//设置搬运长度
        DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);//使能DMA通道
        DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);//使能DMA通道
        DL_TimerA_startCounter(TIMER_0_INST);
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
            DL_TimerA_stopCounter(TIMER_0_INST);
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