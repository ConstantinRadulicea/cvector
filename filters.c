#include "filters.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

void pt1_filter_init_lowpass(pt1_filter_t* filter, float cutoff_freq, float sample_rate) {
	if(cutoff_freq <= FLT_EPSILON || sample_rate <= FLT_EPSILON){
		filter->alpha = 0.0f;
	}
	else{
	    float dt = 1.0f / sample_rate;
	    float rc = 1.0f / (2.0f * M_PI * cutoff_freq);
	    filter->alpha = dt / (dt + rc);
	}
	filter->sample_freq = sample_rate;
    filter->cutoff_freq = cutoff_freq;
    filter->state = 0.0f;
}

float pt1_filter_apply(pt1_filter_t* filter, float input) {
    filter->state += filter->alpha * (input - filter->state);
    return filter->state;
}

void biquad_filter_init_lowpass(biquad_filter_t* f, float cutoff_freq, float sample_freq) {
    float omega = 2.0f * M_PI * cutoff_freq / sample_freq;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float alpha = sn / sqrtf(2.0f);  // Butterworth Q = sqrt(2)/2

    float b0 = (1.0f - cs) / 2.0f;
    float b1 = 1.0f - cs;
    float b2 = (1.0f - cs) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cs;
    float a2 = 1.0f - alpha;

    // Normalize coefficients
    f->b0 = b0 / a0;
    f->b1 = b1 / a0;
    f->b2 = b2 / a0;
    f->a1 = a1 / a0;
    f->a2 = a2 / a0;

    // Clear filter state
    f->z1 = 0.0f;
    f->z2 = 0.0f;

	f->sample_freq = sample_freq;
    f->cutoff_freq = cutoff_freq;
}

float biquad_filter_apply(biquad_filter_t* f, float input) {
    float output = f->b0 * input + f->z1;
    f->z1 = f->b1 * input - f->a1 * output + f->z2;
    f->z2 = f->b2 * input - f->a2 * output;
    return output;
}
