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

 #define ADC_FIFO_SAMPLES (ADC_SAMPLE_SIZE >> 1)
 #define FFT_LENGTH      1024				// FFT长度，默认是1024点FFT
uint16_t ADC_Value[FFT_LENGTH],wavedata[100];
volatile bool gCheckADC;
volatile float32_t max=0,thd=0,samplefre=0,wavefre=0,nor1=1,nor2=0,nor3=0,nor4=0,nor5=0;
volatile uint16_t max_index=0,ave=0;
volatile uint8_t state=0,lenstr=0,uartflag=1;
volatile uint32_t sum=0;
char thdstr[200];
#define FLATTOPWIN_A0 0.21557895
#define FLATTOPWIN_A1 0.41663158
#define FLATTOPWIN_A2 0.277263158
#define FLATTOPWIN_A3 0.083578947
#define FLATTOPWIN_A4 0.006947368

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
void avecalculate(){
    sum=0;
    for(int i=0;i<FFT_LENGTH;i++){
        sum+= ADC_Value[i];
    }
    ave=sum/FFT_LENGTH;
}
 void thdcalculate(float32_t *fft_outputbuf){
    if(state==1){
        for(int i=0;i<50;i++){
            printf("add s0.id,0,%d\xff\xff\xff",ADC_Value[i]/64+10);
        }
        sprintf(thdstr,"wave clear\r\n");
        for(int i=0;i<strlen(thdstr);i++){
            DL_UART_transmitData(UART_2_INST, thdstr[i]);
            delay_cycles(32000);
        }
        sprintf(thdstr,"wave = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",ADC_Value[10],ADC_Value[11],ADC_Value[12],ADC_Value[13],ADC_Value[14],ADC_Value[15],ADC_Value[16],ADC_Value[17],ADC_Value[18],ADC_Value[19],
        ADC_Value[20],ADC_Value[21],ADC_Value[22],ADC_Value[23],ADC_Value[24],ADC_Value[25],ADC_Value[26],ADC_Value[27],ADC_Value[28],ADC_Value[29],
         ADC_Value[30],ADC_Value[31],ADC_Value[32],ADC_Value[33],ADC_Value[34],ADC_Value[35],ADC_Value[36],ADC_Value[37],ADC_Value[38],ADC_Value[39],
          ADC_Value[40],ADC_Value[41],ADC_Value[42],ADC_Value[43],ADC_Value[44],ADC_Value[45],ADC_Value[46],ADC_Value[47],ADC_Value[48],ADC_Value[49],
           ADC_Value[50],ADC_Value[51],ADC_Value[52],ADC_Value[53],ADC_Value[54],ADC_Value[55],ADC_Value[56],ADC_Value[57],ADC_Value[58],ADC_Value[59]);
        for(int i=0;i<strlen(thdstr);i++){
            DL_UART_transmitData(UART_2_INST, thdstr[i]);
            delay_cycles(32000);
        }
    }
    float32_t wave1,wave2,wave3,wave4,wave5;
    float32_t find_max=0;
    for(int i=-2;i<3;i++){
        if(fft_outputbuf[i+max_index]>find_max){
            find_max=fft_outputbuf[i+max_index];
        }
    }
    wave1=find_max;
    find_max=0;
    for(int i=-2;i<3;i++){
        if(fft_outputbuf[i+max_index*2]>find_max){
            find_max=fft_outputbuf[i+max_index*2];
        }
    }
    nor2=find_max/wave1;
    wave2=find_max*find_max;
    find_max=0;
    for(int i=-2;i<3;i++){
        if(fft_outputbuf[i+max_index*3]>find_max){
            find_max=fft_outputbuf[i+max_index*3];
        }
    }
    nor3=find_max/wave1;
    wave3=find_max*find_max;
    find_max=0;
    for(int i=-2;i<3;i++){
        if(fft_outputbuf[i+max_index*4]>find_max){
            find_max=fft_outputbuf[i+max_index*4];
        }
    }
    nor4=find_max/wave1;
    wave4=find_max*find_max;
    find_max=0;
    for(int i=-2;i<3;i++){
        if(fft_outputbuf[i+max_index*5]>find_max){
            find_max=fft_outputbuf[i+max_index*5];
        }
    }
    nor5=find_max/wave1;
    wave5=find_max*find_max;
    thd=sqrt(wave2+wave3+wave4+wave5)/wave1;
    //uartflag=1;
    printf("x0.val=%d\xff\xff\xff",(uint16_t)(thd*1000));
    printf("x1.val=%d\xff\xff\xff",(uint16_t)(nor2*1000));
    printf("x2.val=%d\xff\xff\xff",(uint16_t)(nor3*1000));
    printf("x3.val=%d\xff\xff\xff",(uint16_t)(nor4*1000));
    printf("x4.val=%d\xff\xff\xff",(uint16_t)(nor5*1000));
    //uartflag=2;
    //delay_cycles(320000);
    sprintf(thdstr,"  THD = %.2f\r\n",thd);
    for(int i=0;i<strlen(thdstr);i++){
        DL_UART_transmitData(UART_2_INST, thdstr[i]);
        delay_cycles(32000);
    }
    sprintf(thdstr,"harmonic = %.2f,%.2f,%.2f,%.2f\r\n",nor2,nor3,nor4,nor5);
    for(int i=0;i<strlen(thdstr);i++){
        DL_UART_transmitData(UART_2_INST, thdstr[i]);
        delay_cycles(32000);
    }
   // printf("THD = %.2f\r\n",thd);
 }

 void FFT(float32_t *fft_inputbuff,float32_t *fft_outputbuff)
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
}
int main(void)
{   
    float32_t fft_inputbuffer[FFT_LENGTH*2] ;// FFT输入数组
    float32_t fft_outputbuffer[FFT_LENGTH/2];
    SYSCFG_DL_init();
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    gCheckADC = false;
    samplefre=2000000;
    state=2;
    while (1) {
        if(gCheckADC==true){
            if(state==0){
                avecalculate();
                FFT(fft_inputbuffer,fft_outputbuffer);
                wavefre=samplefre/FFT_LENGTH*max_index;
                if(wavefre<40000){
                    samplefre=wavefre*50;
                }
                else {
                    samplefre=wavefre*50/51;
                }
                
                state=1;
            }
            else if(state==1){
                avecalculate();
                FFT(fft_inputbuffer,fft_outputbuffer);
                thdcalculate(fft_outputbuffer);
                samplefre=2000000;
                state=2;
            }
            else if(state==2){
                avecalculate();
                FFT(fft_inputbuffer,fft_outputbuffer);
                wavefre=samplefre/FFT_LENGTH*max_index;
                samplefre=wavefre*2;
              /*  if(wavefre<40000){
                    samplefre=wavefre*20;
                }
                else {
                    samplefre=wavefre*20/21;
                }*/
                
                state=0;
            }
                DL_Timer_setLoadValue(TIMA0, (uint32_t)32000000/samplefre-1);//////////////0.3887              0.1181
                DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
                DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_Value[0]);
                DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
                DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
                gCheckADC = false;
        }
    }
}
 void ADC12_0_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            gCheckADC = true;
            break;
        default:
            break;
    }
 }