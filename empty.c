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
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979323846
#define ARRAY_SIZE 1024
#define CYCLES 2
#define POINTS_PER_CYCLE (ARRAY_SIZE / CYCLES)
#define NUM_POINTS 100
#define NUM_CYCLES 2
#define AMPLITUDE 512
#define OFFSET 512
#define MAX_VALUE 1024
#define delay_cycles(cycles) DL_Common_delayCycles(cycles)
void generateSineWave(uint16_t data[ARRAY_SIZE]) {
    const double amplitude = 32767.5;  // ???? (65535/2)
    const double offset = 32767.5;     // ֱ��ƫ�� (65535/2)
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // ���㵱ǰ��λ??0 ?? 4��??
        double phase = (double)(i % POINTS_PER_CYCLE) / POINTS_PER_CYCLE * 2 * PI;
        
        // ��������ֵ��ӳ�䵽[0, 65535]��Χ
        double sin_value = sin(phase);
        double value = sin_value * amplitude + offset;
        
        // �������벢ת??Ϊuint16_t
        data[i] = (uint16_t)(value + 0.5);
    }
}

int main(void)
{
    SYSCFG_DL_init();
    uint16_t sine_wave[NUM_POINTS];
    
    // 生成正弦波数据
    for (int i = 0; i < NUM_POINTS; i++) {
        // 计算相位 (0 到 4π)
        double phase = 2.0 * PI * NUM_CYCLES * i / NUM_POINTS;
        
        // 计算正弦值并缩放到0-4096范围
        double value = AMPLITUDE * sin(phase) + OFFSET;
        sine_wave[i]=(uint16_t)value;
        
    }
    LCD_Init();//LCD��???��
    uint16_t waveform[1024];
    for(int i=0;i<NUM_POINTS-1;i++){
        LCD_DrawLine(i, sine_wave[i]/4, i+1, sine_wave[i+1]/4, RED);
    }
    // LCD_ShowString(10,48,"LCD_W:",RED,WHITE,16,0);
    // LCD_ShowString(100,48,"LCD_H:",RED,WHITE,16,0);
    // LCD_ShowString(18,78,"Increaseing",RED,WHITE,16,0);
    // LCD_DrawWaveform(waveform, 1024, 0, 0, 100, 100);
    while (1) {
    }
}
