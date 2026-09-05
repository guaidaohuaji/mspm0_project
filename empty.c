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
uint16_t ADC0_Value[FFT_LENGTH],ADC1_Value[FFT_LENGTH],wavedata[100];
volatile bool gCheckADC0,gCheckADC1;
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


int main(void)
{   
    float32_t fft_inputbuffer[FFT_LENGTH*2] ;// FFT输入数组
    float32_t fft_outputbuffer[FFT_LENGTH/2];
    SYSCFG_DL_init();
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);
    gCheckADC0 = false;
    gCheckADC1 = false;
    samplefre=2000000;
    DL_OPA_setGain(OPA_0_INST,DL_OPA_GAIN_N15_P16);
    state=2;
    while (1) {
        if(gCheckADC0==true){
            DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) 0x40556280);
            DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0_Value[0]);
            DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, FFT_LENGTH);
            DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
            DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) 0x40558280);
            DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1_Value[0]);
            DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, FFT_LENGTH);
            DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
            gCheckADC0 = false;
        }
    }
}
 void ADC12_0_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            gCheckADC0 = true;
            break;
        default:
            break;
    }
 }
 void ADC12_1_INST_IRQHandler(void)
 {
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            gCheckADC1 = true;
            break;
        default:
            break;
    }
 }