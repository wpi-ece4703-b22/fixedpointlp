#include "xlaudio.h"
#include "xlaudio_armdsp.h"

float32_t lpfloat(float32_t x) {
    static float32_t state;
    float32_t y = x + state * 0.5;
    state       = y;
    return y;
}

q15_t lpq15(q15_t x) {
    static int state;                           // accumulate as fix<16,15>
    const int coeff = (int) (0.5 * (1 << 15));  // coeff as <16,15>
    int mul = state * coeff;                    // mul is <32,30>
    state   = x + (mul >> 15);                  // add <16,15> and <17,15>
    int y   = state;                            // return output
    return y;
}

uint16_t processSampleFloat(uint16_t x) {
    return xlaudio_f32_to_dac14(lpfloat(xlaudio_adc14_to_f32(x)));
}

uint16_t processSampleQ15(uint16_t x) {
    return xlaudio_q15_to_dac14(lpq15(xlaudio_adc14_to_q15(x)));
}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    xlaudio_init_intr(FS_16000_HZ, XLAUDIO_J1_2_IN, processSampleQ15);
    xlaudio_run();

    return 1;
}
