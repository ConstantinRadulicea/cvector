#include "motor_mixer.h"
#include "rate_controller.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MIN(MAX(x, lo), hi))

#define PID_LIMIT CONTROLLER_PID_MAX_OUTPUT                 // Axis PID clamp
#define AXIS_TO_MOTOR_GAIN (0.5f / PID_LIMIT)  // ±500 → ±0.5 motor range

void motor_mixer_mix(float throttle, float roll, float pitch, float yaw, float out[NUM_MOTORS]) {
    // Motor order: 0 = FL, 1 = FR, 2 = RL, 3 = RR
    out[0] = throttle + pitch + roll - yaw; // Front Left  (CCW)
    out[1] = throttle + pitch - roll + yaw; // Front Right (CW)
    out[2] = throttle - pitch + roll + yaw; // Rear Left   (CW)
    out[3] = throttle - pitch - roll - yaw; // Rear Right  (CCW)
}

static const float MIX[4][3] = {
    // roll   pitch   yaw
    { +1.0f, -1.0f, +1.0f }, // Front Right (1)
    { -1.0f, -1.0f, -1.0f }, // Front Left  (2)
    { +1.0f, +1.0f, -1.0f }, // Rear Right  (3)
    { -1.0f, +1.0f, +1.0f }  // Rear Left   (4)
};

// Motor mixer for Quad-X
void motor_mixer_quad_x(
    float throttle_norm,       // 0..1 throttle
    float roll_out, float pitch_out, float yaw_out,  // ±PID_LIMIT
	float throttle_idle,  // e.g., 0..1 idle throttle
    float motor_dshot[NUM_MOTORS]     // output throttle per motor
) {
	// Clamp inputs
	throttle_norm = CLAMP(throttle_norm, 0.0f, 1.0f);
	throttle_idle = CLAMP(throttle_idle, 0.0f, 1.0f);
    // Scale PID outputs into motor fraction
    float r = roll_out * AXIS_TO_MOTOR_GAIN;
    float p = pitch_out * AXIS_TO_MOTOR_GAIN;
    float y = yaw_out * AXIS_TO_MOTOR_GAIN;

    // Step 1: Compute raw motor mix
    float m[4];


    for (int i = 0; i < 4; i++) {
        m[i] = throttle_norm + MIX[i][0] * r + MIX[i][1] * p + MIX[i][2] * y;
    }

    // Step 2: Desaturate so all motors stay in [0..1]
    float mmin = m[0], mmax = m[0];
    for (int i = 1; i < 4; i++) {
        if (m[i] < mmin) mmin = m[i];
        if (m[i] > mmax) mmax = m[i];
    }

    if (mmax > 1.0f) {
        float shift = mmax - 1.0f;
        for (int i = 0; i < 4; i++) m[i] -= shift;
    }
    if (mmin < 0.0f) {
        float shift = 0.0f - mmin;
        for (int i = 0; i < 4; i++) m[i] += shift;
    }

    for (int i = 0; i < 4; i++) {
        m[i] = CLAMP(m[i], throttle_idle, 1.0f);
        motor_dshot[i] = m[i];
    }
}