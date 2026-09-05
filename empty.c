/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
 #include "stdio.h"
 #include <string.h>
 #include <stdarg.h>
 #include "arm_math.h"
 #include "arm_const_structs.h"
 #include "stdlib.h"

 #define ADC_FIFO_SAMPLES (ADC_SAMPLE_SIZE >> 1)
 #define FFT_LENGTH      1024				// FFT长度，默认是1024点FFT
 #define gate        20
volatile uint16_t ADC0_Value[FFT_LENGTH],ADC1_Value[FFT_LENGTH],wavedata[500];
volatile uint16_t adc1count=0,adc0count=0,startindex=0,endindex=0,wavelen=0,startbuff=0,endbuff=0,wavelencount=0,sumcount=0;
volatile bool adc0flag=false,adc1flag=false,windowflag=false,waveoutflag=false;
volatile float32_t max=0,thd=0,samplefre=0,wavefre=0;
volatile uint16_t max_index=0,ave=0;
volatile uint8_t state=0,lenstr=0,uartflag=1,pp=0;
volatile uint32_t sum=0,timestart=0,timeend=0;
#define FLATTOPWIN_A0 0.21557895
#define FLATTOPWIN_A1 0.41663158
#define FLATTOPWIN_A2 0.277263158
#define FLATTOPWIN_A3 0.083578947
#define FLATTOPWIN_A4 0.006947368

int fputc(int ch, FILE* stream)
{
    DL_UART_Main_transmitDataBlocking(UART_1_INST, ch);
    return ch;
}
int puts(const char *_ptr)
{
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout);
    return count;
}
int fputs(const char* restrict s, FILE* restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i=0; i<len; i++)
    {
        DL_UART_Main_transmitDataBlocking(UART_1_INST, s[i]);
    }
    return len;
}

void adchandle(uint16_t *adcvalue){
    sum=0;
    sumcount=0;
    for(int i=0;i<FFT_LENGTH;i++){
        if(adcvalue[i]>=1200&&adcvalue[i]<=1300){
            sum+= adcvalue[i];
            sumcount++;
        }
    }
    ave=sum/sumcount;
    startbuff=2048;
    endbuff=2048;
    for(int i=0;i<1021;i++){
        if(abs(adcvalue[i]-ave)<=gate&&abs(adcvalue[i+1]-ave)>gate&&abs(adcvalue[i+2]-ave)>gate&&abs(adcvalue[i+3]-ave)>gate&&startbuff==2048){
            startbuff=i;
        }
        if(abs(adcvalue[i]-ave)>gate&&abs(adcvalue[i+1]-ave)<=gate&&abs(adcvalue[i+2]-ave)<=gate&&abs(adcvalue[i+3]-ave)<=gate&&startbuff!=2048&&endbuff==2048){
            endbuff=i;
            if(endbuff-startbuff>wavelen){
                startindex=startbuff;
                endindex=endbuff;
                wavelen=endindex-startindex;
                wavelencount=0;
                if(wavelen>20){
                    for(int i=startindex;i<=endindex;i++){
                        wavedata[wavelencount]=adcvalue[i];
                        wavelencount++;
                    }
                    waveoutflag=true;
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&wavedata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,wavelen+1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
            }
            startbuff=2048;
            endbuff=2048;
        }
    }
}
 /*void FFT(float32_t *fft_inputbuff,float32_t *fft_outputbuff)
{

	for(int i=0;i < FFT_LENGTH;i++)
	{   
        if(state==0){
            fft_inputbuff[i*2] = ((float)ADC_Value[i]-ave)*0.5-0.5*cos((2 * PI*(i - 1))/(FFT_LENGTH - 1));;
        }
		else {
            fft_inputbuff[i*2] = ((float)ADC_Value[i]-ave)*(FLATTOPWIN_A0 - FLATTOPWIN_A1 * cos(2 * PI * i / (FFT_LENGTH - 1))+ FLATTOPWIN_A2 * cos(4 * PI * i / (FFT_LENGTH - 1))- FLATTOPWIN_A3 * cos(6 * PI * i / (FFT_LENGTH - 1))+ FLATTOPWIN_A4 * cos(8 * PI * i / (FFT_LENGTH - 1)));
        }
		fft_inputbuff[2*i +1] = 0;
	}
	arm_cfft_f32(&arm_cfft_sR_f32_len1024,fft_inputbuff,0,1); // 执行FFT变换，arm_cfft_sR_f32_len128为宏，定义旋转因子
	arm_cmplx_mag_f32(fft_inputbuff,fft_outputbuff,FFT_LENGTH);    // 把运算结果复数求模得幅值
    max_index=0;
    max=0;
    for(int i=0;i<FFT_LENGTH/2;i++){
        fft_outputbuff[i]/=512;
        if(i>=3&&fft_outputbuff[i]>max){
            max_index=i;
            max=fft_outputbuff[i];
        }
    }
}*/
int main(void)
{   
    float32_t fft_inputbuffer[FFT_LENGTH*2] ;// FFT输入数组
    float32_t fft_outputbuffer[FFT_LENGTH/2];
    SYSCFG_DL_init();
    

    
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);
    DL_TimerA_startCounter(TIMER_0_INST);
    DL_Common_delayCycles(3200);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    DL_TimerA_startCounter(TIMER_2_INST);
    DL_TimerA_startCounter(TIMER_1_INST);
   /// DL_Common_delayCycles(16000);
  //  DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    adc0flag = false;
    adc1flag = false;
    windowflag=false;
    state=0;
    samplefre=2000000;
    while (1) {
        if(windowflag==true){
            adchandle(ADC1_Value);
            adchandle(ADC0_Value);
            windowflag=false;
        }
    }
}
 void ADC12_0_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            if(windowflag==false){
                DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
                DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
                DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
                DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
            }
            break;
        default:
            break;
    }
 }
  void ADC12_1_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            if(windowflag==false){
                DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
                DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
                DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
                DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
            }
            
            break;
        case DL_ADC12_IIDX_WINDOW_COMP_HIGH:
            if(windowflag==false){
                //DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
                //DL_DMA_disableChannel(DMA, DMA_CH1_CHAN_ID);
                windowflag=true;
            }
            break;
        /*case DL_ADC12_IIDX_WINDOW_COMP_LOW:
            if(windowflag==false){
            DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
            DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
            DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
            DL_ADC12_enableDMA(ADC12_0_INST);
            windowflag=true;
        }
            
            break;*/
        default:
            break;
    }
 }