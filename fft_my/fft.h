#ifndef _FFT_H
#define _FFT_H

 #include "ti_msp_dl_config.h"
 #include "arm_math.h"
 #include "arm_const_structs.h"
  #define LENGTH      512

/*void hanning_window(float *w);
void convolve(float *w, float *y);

float sum(float *x, int n);
void normalize(float *x, int n);*/
float Phase_atan(float32_t *inputSignal,uint32_t index);
void AP_FFT(uint16_t *ADC_Value);
 #endif