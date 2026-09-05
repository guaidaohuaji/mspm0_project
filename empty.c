
#include "ti_msp_dl_config.h"
#include "AD9959.h"
#include "fft.h"
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "arm_math.h"
#include "arm_const_structs.h"

#define LENGTH      512
#define PI 3.14159265358979323846
#define NUM_POINTS 100
#define NUM_CYCLES 2
#define AMPLITUDE 512
#define OFFSET 512
#define MAX_VALUE 1024
#define delay_cycles(cycles) DL_Common_delayCycles(cycles)
volatile uint16_t mode=0,index_1=0,index_2=0,pwmx=36800,pwmy=37000,count=0;
volatile uint8_t length=0;
uint16_t ADC0_Value[LENGTH*2],ADC1_Value[LENGTH*2];
volatile bool ADC0FLAG,ADC1FLAG;
volatile uint16_t vpp1,vpp2;
volatile float Phase_1=0,Phase_2=0,Phase=0,fmax_1=0,fmax_2=0;
uint16_t max_1=0,min_1=4096;

float HanNing[LENGTH]={0};                                           //汉宁窗数组
float fft_conv[2*LENGTH-1]={0};    

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
    float32_t fft_inputbuff_1[LENGTH*2] ;// FFT输入数组
    float32_t fft_outputbuff_1[LENGTH/2];
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART1_INT_IRQn);
   // hanning_window(HanNing);
    //convolve(HanNing,fft_conv);
    //normalize(fft_conv,2*LENGTH-1);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);//设置ADC对应的内存地址
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);//设置搬运终点
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, LENGTH*2);//设置搬运长度
   // DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);//设置ADC对应的内存地址
    //DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);//设置搬运终点
    //DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, LENGTH*2);//设置搬运长度
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);//使能DMA通道
    //DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);//使能DMA通道
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);//使能中断
   // NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);//使能中断
    ADC0FLAG=false;
    ADC1FLAG=false;
    uint16_t sine_wave[NUM_POINTS];
    
    // 生成正弦波数据
    for (int i = 0; i < NUM_POINTS; i++) {
        // 计算相位 (0 到 4π)
        double phase = 2.0 * PI * NUM_CYCLES * i / NUM_POINTS;
        
        // 计算正弦值并缩放到0-4096范围
        double value = AMPLITUDE * sin(phase) + OFFSET;
        sine_wave[i]=(uint16_t)value;
        
    }
    DL_TimerA_startCounter(TIMER_0_INST);
    DL_TimerG_startCounter(PWM_0_INST);
    
    while (1) {
        
      if(ADC0FLAG==true){
        DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
        ADC0FLAG=false;
        ADC1FLAG=false;
        for(int i=0;i<100;i++){
            pwmx+=8;
            pwmy=37150-ADC0_Value[i*2]+512;
            count++;
            DL_TimerG_setCaptureCompareValue(PWM_0_INST,pwmx,DL_TIMER_CC_0_INDEX);
            DL_TimerG_setCaptureCompareValue(PWM_0_INST,pwmy,DL_TIMER_CC_1_INDEX);
            delay_cycles(4000000);
        }
        pwmx=36800;
        pwmy=37150;
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,38000,DL_TIMER_CC_0_INDEX);
        delay_cycles(40000000);
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,39000,DL_TIMER_CC_1_INDEX);
        delay_cycles(40000000);
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,35200,DL_TIMER_CC_0_INDEX);
        delay_cycles(40000000);
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,37150,DL_TIMER_CC_1_INDEX);
        delay_cycles(40000000);
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,36800,DL_TIMER_CC_0_INDEX);
        delay_cycles(40000000);
        DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);//设置ADC对应的内存地址
        DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);//设置搬运终点
        DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, LENGTH*2);//设置搬运长度
        //DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);//设置ADC对应的内存地址
        //DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);//设置搬运终点
        //DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, LENGTH*2);//设置搬运长度
        DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);//使能DMA通道
        //DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);//使能DMA通道
        DL_TimerA_startCounter(TIMER_0_INST);
      }
    }
}
void ADC12_0_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            ADC0FLAG = true;
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