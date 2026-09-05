 #include "fft.h"

 #define LENGTH      512
extern float HanNing[LENGTH];                                           //汉宁窗数组
extern float fft_conv[2*LENGTH-1];    
extern uint16_t vpp1,vpp2,index_1,index_2;
extern float Phase_1,Phase_2,Phase,fmax_1,fmax_2;
extern uint16_t max_1,min_1;
extern float32_t fft_inputbuf_1[LENGTH*2] ;// FFT输入数组
extern float32_t fft_outputbuf_1[LENGTH/2];
extern float32_t fft_inputbuf_2[LENGTH*2] ;// FFT输入数组
extern float32_t fft_outputbuf_2[LENGTH/2];
void hanning_window(float *w)
{
    uint16_t n;
    for (n = 0; n < LENGTH; n++)
    {
        w[n] = 0.5 * (1 - cos(2 * PI * n / LENGTH));
    }
}
 
void convolve(float *w, float *y)
{
    int n, m;
    for (n = 0; n < 2 * LENGTH - 1; n++) {
        y[n] = 0;
        for (m = 0; m < LENGTH; m++) {
            if (n - m >= 0 && n - m < LENGTH) {
                y[n] += w[m] * w[n - m];
            }
        }
    }
}
 
// 计算序列的和
float sum(float *x, int n)
{
    int i;
    float s = 0;
    for (i = 0; i < n; i++) {
        s += x[i];
    }
    return s;
}
 
// 归一化序列
void normalize(float *x, int n)
{
    float s = sum(x, n);
    int i;
    for (i = 0; i < n; i++) {
        x[i] /= s;
    }
}
//相位计算函数
float Phase_atan(float32_t *inputSignal,uint32_t index)
{
    if(inputSignal[2*index+1] >= 0 && inputSignal[2*index] >= 0)
        return   0 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    else if(inputSignal[2*index+1] >= 0 && inputSignal[2*index] <= 0)
        return 180 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    else if(inputSignal[2*index+1] <= 0 && inputSignal[2*index] <= 0)
        return 180 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    else if(inputSignal[2*index+1] <= 0 && inputSignal[2*index] >= 0)
        return 360 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
    return 0;
}
void AP_FFT(uint16_t *wave_1,float32_t *fft_inputbuf_1,float32_t* fft_outputbuf_1)
{   
    max_1=0,min_1=4096;
    float midwave[1024];
    for(int i = 0; i < 2*LENGTH-1; i++)
    {   
        if(wave_1[i]>max_1){
            max_1=wave_1[i];
        }
        if(wave_1[i]<min_1){
            min_1=wave_1[i];
        }
        
        midwave[i] = (float)wave_1[i] * fft_conv[i];    //加窗

    }
//   Printf(fft_conv,Length*2-1);
    for(int i = 0; i < LENGTH-1; i++)
    {
        midwave[i] = midwave[i] + midwave[LENGTH+i];
    }
//    Printf(wave_1,Length);
    for (int i = 0; i < LENGTH; i++)
    {
        fft_inputbuf_1[i * 2] = midwave[i];
        fft_inputbuf_1[i * 2 + 1] = 0;
    }
 	arm_cfft_f32(&arm_cfft_sR_f32_len512,fft_inputbuf_1,0,1); 
    arm_cmplx_mag_f32(fft_inputbuf_1, fft_outputbuf_1, LENGTH);
 
    fft_outputbuf_1[0] = 0;
 
    for(int h=0;h<LENGTH/2;h++)
    {
        fft_outputbuf_1[h]/=LENGTH/2;

        if(h>=3&&fft_outputbuf_1[h]>fmax_1){
            index_1=h;
            fmax_1=fft_outputbuf_1[h];
        }

    }

                 //根据实际情况补偿，非必要不加


}