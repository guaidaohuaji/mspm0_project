#ifndef _FFT_H
#define _FFT_H

 #include "ti_msp_dl_config.h"
 #include "arm_math.h"
 #include "arm_const_structs.h"

void hanning_window(float *w);
void convolve(float *w, float *y);
float sum(float *x, int n);
void normalize(float *x, int n);
float Phase_atan(float32_t *inputSignal,uint32_t index);
void AP_FFT(uint16_t *wave_1,float32_t *fft_inputbuf_1,float32_t* fft_outputbuf_1);
 #endif