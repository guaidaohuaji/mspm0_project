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
 #define gate        30
volatile uint16_t ADC0_Value[FFT_LENGTH],ADC1_Value[FFT_LENGTH],wave1data[FFT_LENGTH],wave2data[FFT_LENGTH],wave3data[500],wave4data[500],buffdata[500]={0},outputdata[500],fftdata[FFT_LENGTH];
volatile uint16_t startindex=0,endindex=0,wavelen=0,startbuff=0,endbuff=0,wavelencount=0,wave1len=0,wave2len=0,wave3len=0,wave4len=0;
volatile bool windowflag=false,waveoutflag=false,changewave=false;
volatile float32_t max=0,top2=0,wavefre=0,outfre=0,top3=0,top2fre=0,top3fre=0,top4=0,top5=0,top4fre=0,top5fre=0;
volatile uint16_t max_index=0,ave=0,ave_index=0,waveam=0,base=0,top2_index=0,top3_index=0,top2am=0,top3am=0,top4am=0,top5am=0,top4_index=0,top5_index=0;
volatile uint8_t state=0,lenstr=0,uartflag=1,pp=0,useadc=0,useadccount=0,mode=0,inputwave=1,outputwave=1,uartreceive=0;
volatile uint32_t sum=0,adc0fre=2500000,adc1fre=500000;
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

////////////////////////////////////////adc数据处理





void adchandle(uint16_t *adcvalue){
    startbuff=2048;
    endbuff=2048;
    if(mode==2){
        if(useadccount==2){
            useadc=0;
            uint16_t max_2=0,max_index_2=0;
            for(int i=0;i<FFT_LENGTH;i++){
                if(adcvalue[i]>max_2){
                    max_2=adcvalue[i];
                }
            }
            for(int i=0;i<FFT_LENGTH;i++){
                if(abs(adcvalue[i]-max_2)<=20&&startbuff!=2048){
                    endbuff=i;
                }
                if(abs(adcvalue[i]-max_2)<=20&&startbuff==2048){
                    startbuff=i;
                }
            }
            if(inputwave==1){
                if(endbuff-startbuff>wave1len){
                    startindex=startbuff;
                    endindex=endbuff;
                    wave1len=endindex-startindex;
                    wavelencount=0;
                    if(wave1len>20){
                        for(int i=startindex;i<=endindex;i++){
                            wave1data[wavelencount]=adcvalue[i];
                            wavelencount++;
                        }
                        waveoutflag=true;
                        if(useadccount==1){
                            useadc=1;
                        }
                        else if(useadccount==2){
                            useadc=0;
                        }
                    }
                }
            }
            else if(inputwave==2){
                if(endbuff-startbuff>wave2len){
                    startindex=startbuff;
                    endindex=endbuff;
                    wave2len=endindex-startindex;
                    wavelencount=0;
                    if(wave2len>20){
                        for(int i=startindex;i<=endindex;i++){
                            wave2data[wavelencount]=adcvalue[i];
                            wavelencount++;
                        }
                        waveoutflag=true;
                        if(useadccount==1){
                            useadc=1;
                        }
                        else if(useadccount==2){
                            useadc=0;
                        }
                    }
                }
            }
            else if(inputwave==3){
                if(endbuff-startbuff>wave3len){
                    startindex=startbuff;
                    endindex=endbuff;
                    wave3len=endindex-startindex;
                    wavelencount=0;
                    if(wave3len>20){
                        for(int i=startindex;i<=endindex;i++){
                            wave3data[wavelencount]=adcvalue[i];
                            wavelencount++;
                        }
                        waveoutflag=true;
                        if(useadccount==1){
                            useadc=1;
                        }
                        else if(useadccount==2){
                            useadc=0;
                        }
                    }
                }
            }
            else if(inputwave==4){
                if(endbuff-startbuff>wave4len){
                    startindex=startbuff;
                    endindex=endbuff;
                    wave4len=endindex-startindex;
                    wavelencount=0;
                    if(wave4len>20){
                        for(int i=startindex;i<=endindex;i++){
                            wave4data[wavelencount]=adcvalue[i];
                            wavelencount++;
                        }
                        waveoutflag=true;
                        if(useadccount==1){
                            useadc=1;
                        }
                        else if(useadccount==2){
                            useadc=0;
                        }
                    }
                }
            }
            startbuff=2048;
            endbuff=2048;
        }
    }
    else {
        ave=0;
        for(int i=0;i<FFT_LENGTH;i++){
            ave_index=adcvalue[i]/10;
            buffdata[ave_index]++;
        }
        for(int i=0;i<410;i++){
            if(ave<buffdata[i]){
                ave=buffdata[i];
                ave_index=i;
            }
            buffdata[i]=0;
        }
        ave=ave_index*10+5;
        for(int i=1;i<1021;i++){
            if(abs(adcvalue[i-1]-ave)<=gate&&abs(adcvalue[i]-ave)<=gate&&abs(adcvalue[i+1]-ave)>gate&&abs(adcvalue[i+2]-ave)>gate&&abs(adcvalue[i+3]-ave)>gate&&startbuff==2048&&((adcvalue[i+1]>=adcvalue[i]&&adcvalue[i+2]>=adcvalue[i]&&adcvalue[i+3]>=adcvalue[i])||(adcvalue[i+1]<=adcvalue[i]&&adcvalue[i+2]<=adcvalue[i]&&adcvalue[i+3]<=adcvalue[i]))){
                startbuff=i;
            }
            if(abs(adcvalue[i-1]-ave)>gate&&abs(adcvalue[i]-ave)>gate&&abs(adcvalue[i+1]-ave)<=gate&&abs(adcvalue[i+2]-ave)<=gate&&abs(adcvalue[i+3]-ave)<=gate&&startbuff!=2048&&endbuff==2048&&((adcvalue[i+1]>=adcvalue[i]&&adcvalue[i+2]>=adcvalue[i]&&adcvalue[i+3]>=adcvalue[i])||(adcvalue[i+1]<=adcvalue[i]&&adcvalue[i+2]<=adcvalue[i]&&adcvalue[i+3]<=adcvalue[i]))){
                endbuff=i+1;
                if(inputwave==1){
                    if(endbuff-startbuff>wave1len){
                        startindex=startbuff;
                        endindex=endbuff;
                        wave1len=endindex-startindex;
                        wavelencount=0;
                        if(wave1len>20){
                            for(int i=startindex;i<=endindex;i++){
                                wave1data[wavelencount]=adcvalue[i];
                                wavelencount++;
                            }
                            waveoutflag=true;
                            if(useadccount==1){
                                useadc=1;
                            }
                            else if(useadccount==2){
                                useadc=0;
                            }
                        }
                    }
                }
                else if(inputwave==2){
                    if(endbuff-startbuff>wave2len){
                        startindex=startbuff;
                        endindex=endbuff;
                        wave2len=endindex-startindex;
                        wavelencount=0;
                        if(wave2len>20){
                            for(int i=startindex;i<=endindex;i++){
                                wave2data[wavelencount]=adcvalue[i];
                                wavelencount++;
                            }
                            waveoutflag=true;
                            if(useadccount==1){
                                useadc=1;
                            }
                            else if(useadccount==2){
                                useadc=0;
                            }
                        }
                    }
                }
                else if(inputwave==3){
                    if(endbuff-startbuff>wave3len){
                        startindex=startbuff;
                        endindex=endbuff;
                        wave3len=endindex-startindex;
                        wavelencount=0;
                        if(wave3len>20){
                            for(int i=startindex;i<=endindex;i++){
                                wave3data[wavelencount]=adcvalue[i];
                                wavelencount++;
                            }
                            waveoutflag=true;
                            if(useadccount==1){
                                useadc=1;
                            }
                            else if(useadccount==2){
                                useadc=0;
                            }
                        }
                    }
                }
                else if(inputwave==4){
                    if(endbuff-startbuff>wave4len){
                        startindex=startbuff;
                        endindex=endbuff;
                        wave4len=endindex-startindex;
                        wavelencount=0;
                        if(wave4len>20){
                            for(int i=startindex;i<=endindex;i++){
                                wave4data[wavelencount]=adcvalue[i];
                                wavelencount++;
                            }
                            waveoutflag=true;
                            if(useadccount==1){
                                useadc=1;
                            }
                            else if(useadccount==2){
                                useadc=0;
                            }
                        }
                    }
                }
                startbuff=2048;
                endbuff=2048;
            }
        }
    }
}






 void FFT(float32_t *fft_inputbuff,float32_t *fft_outputbuff,uint16_t *ADC_Value)
{
    sum=0;
    for(int i=0;i<FFT_LENGTH;i++){
        sum+=fftdata[i];
    }
    ave=sum/FFT_LENGTH;
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
    top2=0;
    top2_index=0;
    top3=0;
    top3_index=0;
    top4=0;
    top4_index=0;
    top5=0;
    top5_index=0;
    for(int i=0;i<FFT_LENGTH/2;i++){
        fft_outputbuff[i]/=512;
        if(i>0&&fft_outputbuff[i]>max){
            max_index=i;
            max=fft_outputbuff[i];
            if(fft_outputbuff[i]>7&&base==0){
                base=i;
            }
        }
    }
    for(int i=-2;i<3;i++){
        if(fft_outputbuff[i+max_index*2]>top2){
            top2=fft_outputbuff[i+max_index*2];
            top2_index=i+max_index*2;
        }
    }
    for(int i=-2;i<3;i++){
        if(fft_outputbuff[i+max_index*3]>top3){
            top3=fft_outputbuff[i+max_index*3];
            top3_index=i+max_index*3;
        }
    }
    for(int i=-2;i<3;i++){
        if(fft_outputbuff[i+max_index*4]>top4){
            top4=fft_outputbuff[i+max_index*4];
            top4_index=i+max_index*4;
        }
    }
    for(int i=-2;i<3;i++){
        if(fft_outputbuff[i+max_index*5]>top5){
            top5=fft_outputbuff[i+max_index*5];
            top5_index=i+max_index*5;
        }
    }
}







int main(void)
{   
    float32_t fft_inputbuffer[FFT_LENGTH*2] ;// FFT输入数组
    float32_t fft_outputbuffer[FFT_LENGTH/2];
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART1_INT_IRQn);
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);
    DL_TimerA_startCounter(TIMER_2_INST);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    DL_TimerG_startCounter(TIMER_0_INST);
    windowflag=false;
    state=0;
    while (1) {
        if(windowflag==true){//进行波形抓取根据当前选择的输入数组将波形数据抓取出来存到对应的数组中
            useadccount=0;
            if(mode==0){
                useadccount++;
                adchandle(ADC1_Value);
            }
            else if(mode==2){
                useadccount=2;
                adchandle(ADC0_Value);
            }
            else {
                useadccount++;
                adchandle(ADC1_Value);
                useadccount++;
                adchandle(ADC0_Value);
            }
            windowflag=false;
            DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
            DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
            DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
            DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
            DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
            DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
            DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
            DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
            DL_TimerA_startCounter(TIMER_2_INST);
            DL_TimerG_startCounter(TIMER_0_INST);
        }
        if(changewave==true){//每次按下输出后触发，根据选择输出的波形，对已经存好的输入波形数据进行fft运算，插值输出等操作
            changewave=false;
            for(int i=0;i<FFT_LENGTH;i++){
                if(outputwave==1){
                    fftdata[i]=wave1data[i%(wave1len+1)];
                }
                else if(outputwave==2){
                    fftdata[i]=wave2data[i%(wave2len+1)];
                }
                else if(outputwave==3){
                    fftdata[i]=wave3data[i%(wave3len+1)];
                }
                else if(outputwave==4){
                    fftdata[i]=wave4data[i%(wave4len+1)];
                }
            }
            state=0;
            FFT(fft_inputbuffer,fft_outputbuffer,fftdata);
            if(useadc==1){
                wavefre=max_index*500000/1024;
                top2fre=top2_index*500000/1024;
                top3fre=top3_index*500000/1024;
                top4fre=top4_index*500000/1024;
                top5fre=top5_index*500000/1024;
            }
            else if(useadc==0) {
                wavefre=max_index*2500000/1024;
                top2fre=top2_index*2500000/1024;
                top3fre=top3_index*2500000/1024;
                top4fre=top4_index*2500000/1024;
                top5fre=top5_index*2500000/1024;
            }
            printf("n5.val=%d\xff\xff\xff",(uint16_t)wavefre);
            printf("n6.val=%d\xff\xff\xff",(uint16_t)top3fre);
            printf("n7.val=%d\xff\xff\xff",(uint16_t)top2fre);
            printf("n8.val=%d\xff\xff\xff",(uint16_t)top4fre);
            printf("n9.val=%d\xff\xff\xff",(uint16_t)top5fre);
            useadc=0;
            useadccount=0;
            state=1;
            FFT(fft_inputbuffer,fft_outputbuffer,fftdata);
            waveam=max*9.425/4096*3.3*1000;
            top2am=top2*9.425/4096*3.3*1000;
            top3am=top3*9.425/4096*3.3*1000;
            top4am=top4*9.425/4096*3.3*1000;
            top5am=top5*9.425/4096*3.3*1000;
            if(mode!=1){
                waveam=waveam/0.75/2;
                top2am=top2am/0.75/2;
                top3am=top3am/0.75/2;
                top4am=top4am/0.75/2;
                top5am=top5am/0.75/2;
            }
            else{
                waveam=waveam/28.26/2;
                top2am=top2am/28.26/2;
                top3am=top3am/28.26/2;
                top4am=top4am/28.26/2;
                top5am=top5am/28.26/2;
            }
            printf("n0.val=%d\xff\xff\xff",(uint16_t)(waveam));
            printf("n1.val=%d\xff\xff\xff",(uint16_t)(top3am));
            printf("n2.val=%d\xff\xff\xff",(uint16_t)(top2am));
            printf("n3.val=%d\xff\xff\xff",(uint16_t)(top4am));
            printf("n4.val=%d\xff\xff\xff",(uint16_t)(top5am));
            if(mode==2){
                if(useadc==1){
                    outfre=base*500000/1024;
                }
                else if(useadc==0) {
                    outfre=base*2500000/1024;
                }
                base=0;
            }
            else if(mode==0){
                outfre=5000;
            }
            else if(mode==1){
                outfre=20000;
            }
            if(outputwave==1){
                if(wave1len<100){
                    outputdata[0]=wave1data[0];
                    for(int i=1;i<wave1len;i++){
                        for(int j=0;j<=1;j++){
                            if(j==0){
                                outputdata[i*2+j-1]=(wave1data[i-1]+wave1data[i])/2;
                            }
                            else {
                                outputdata[i*2+j-1]=wave1data[i];
                            }
                        }
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(2*wave1len-1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,2*wave1len-1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
                else {
                    for(int i=0;i<=wave1len;i++){
                        outputdata[i]=wave1data[i];
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(wave1len+1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,wave1len+1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
            }
            else if(outputwave==2){
                if(wave2len<100){
                    outputdata[0]=wave2data[0];
                    for(int i=1;i<wave2len;i++){
                        for(int j=0;j<=1;j++){
                            if(j==0){
                                outputdata[i*2+j-1]=(wave2data[i-1]+wave2data[i])/2;
                            }
                            else {
                                outputdata[i*2+j-1]=wave2data[i];
                            }
                        }
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(2*wave2len-1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,2*wave2len-1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
                else {
                    for(int i=0;i<=wave2len;i++){
                        outputdata[i]=wave2data[i];
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(wave2len+1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,wave2len+1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
            }
            else if(outputwave==3){
                if(wave3len<100){
                    outputdata[0]=wave3data[0];
                    for(int i=1;i<wave3len;i++){
                        for(int j=0;j<=1;j++){
                            if(j==0){
                                outputdata[i*2+j-1]=(wave3data[i-1]+wave3data[i])/2;
                            }
                            else {
                                outputdata[i*2+j-1]=wave3data[i];
                            }
                        }
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(2*wave3len-1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,2*wave3len-1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
                else {
                    for(int i=0;i<=wave3len;i++){
                        outputdata[i]=wave3data[i];
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(wave3len+1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,wave3len+1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
            }
            else if(outputwave==4){
                if(wave4len<100){
                    outputdata[0]=wave4data[0];
                    for(int i=1;i<wave4len;i++){
                        for(int j=0;j<=1;j++){
                            if(j==0){
                                outputdata[i*2+j-1]=(wave4data[i-1]+wave4data[i])/2;
                            }
                            else {
                                outputdata[i*2+j-1]=wave4data[i];
                            }
                        }
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(2*wave4len-1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,2*wave4len-1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
                else {
                    for(int i=0;i<=wave4len;i++){
                        outputdata[i]=wave4data[i];
                    }
                    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                    DL_Timer_setLoadValue(TIMA0, (uint32_t)80000000/(outfre*(wave4len+1))-1);
                    DL_DMA_setSrcAddr(DMA,DMA_CH2_CHAN_ID,(uint32_t )&outputdata[0]);
                    DL_DMA_setDestAddr(DMA,DMA_CH2_CHAN_ID, (uint32_t )&(DAC0->DATA0));
                    DL_DMA_setTransferSize(DMA,DMA_CH2_CHAN_ID,wave4len+1);
                    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
                }
            }
        }
            
    }
}






 void ADC12_0_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            if(windowflag==false){
               // DL_Timer_setLoadValue(TIMA1, (uint32_t)80000000/adc0fre-1);
                DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
                DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
                DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
                DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
            }
            break;
        case DL_ADC12_IIDX_WINDOW_COMP_HIGH:
            if(windowflag==false){
                windowflag=true;
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
               // DL_Timer_setLoadValue(TIMG0, (uint32_t)80000000/adc1fre-1);
                DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
                DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
                DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
                DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
            }
            
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
            uartreceive = DL_UART_Main_receiveData(UART_1_INST);
            if(uartreceive>>4==1){
                inputwave=uartreceive%16;
                if(inputwave==1){
                    wave1len=0;
                }
                else if(inputwave==2){
                    wave2len=0;
                }
                else if(inputwave==3){
                    wave3len=0;
                }
                else if(inputwave==4){
                    wave4len=0;
                }
            }
            else if(uartreceive>>4==2){
                outputwave=uartreceive%16;
                changewave=true;
            }
            else if(uartreceive>>4==3){
                mode=uartreceive%16;
                if(mode==0){
                    DL_GPIO_clearPins(GPIO_GRP_0_PORT,GPIO_GRP_0_PIN_0_PIN);
                }
                else if(mode==1){
                    DL_GPIO_setPins(GPIO_GRP_0_PORT,GPIO_GRP_0_PIN_0_PIN);
                }
                else if(mode==2){
                    DL_GPIO_clearPins(GPIO_GRP_0_PORT,GPIO_GRP_0_PIN_0_PIN);
                }
            }
            break;

        default://其他的串口中断
            break;
    }
    
}