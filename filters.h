#ifndef FILTERS_H
#define FILTERS_H


#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------
// PT1 Filter Struct and API
// -----------------------------
typedef struct {
	float sample_freq;
    float cutoff_freq;  // Hz
    float alpha;        // Precomputed smoothing factor
    float state;        // Last output value
} pt1_filter_t;

/**
 * @brief Initialize a PT1 low-pass filter
 * @param filter Pointer to filter object
 * @param cutoff_freq Desired cutoff frequency in Hz
 * @param sample_rate Sampling rate in Hz
 */
void pt1_filter_init_lowpass(pt1_filter_t* filter, float cutoff_freq, float sample_rate);

/**
 * @brief Apply the PT1 filter to a new input sample
 * @param filter Pointer to filter object
 * @param input New input sample
 * @return Filtered output
 */
float pt1_filter_apply(pt1_filter_t* filter, float input);

void pt1_filter_reset(pt1_filter_t* filter);


typedef struct {
	float sample_freq;
    float cutoff_freq;  // Hz
    float b0, b1, b2;   // Feedforward coefficients
    float a1, a2;       // Feedback coefficients
    float z1, z2;       // Filter state (delays)
} biquad_filter_t;

void biquad_filter_init_lowpass(biquad_filter_t* f, float cutoff_freq, float sample_freq);
float biquad_filter_apply(biquad_filter_t* f, float input);

void biquad_filter_reset(biquad_filter_t* filter);


#ifdef __cplusplus
}
#endif

#endif // FILTERS_H
