#ifndef __MOTOR_MIXER_H__
#define __MOTOR_MIXER_H__

#include <stdint.h>

#define NUM_MOTORS 4

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Mixes control inputs (throttle, roll, pitch, yaw) into individual motor outputs for a quadcopter.
 *
 * This function implements motor mixing for a standard "X" quadcopter layout:
 *
 *   Motor layout (top view):
 *
 *          FRONT
 *           ↑
 *      0         1
 *     (FL)     (FR)
 *
 *      2         3
 *     (RL)     (RR)
 *           ↓
 *          REAR
 *
 * Motor directions (assuming standard quadcopter):
 *   - Motor 0 (FL): CCW
 *   - Motor 1 (FR): CW
 *   - Motor 2 (RL): CCW
 *   - Motor 3 (RR): CW
 *
 * Each motor output is computed as:
 *   output = throttle ± pitch ± roll ± yaw
 *
 * @param throttle Base throttle value in range [0.0, 1.0]
 * @param roll     Roll control command (positive = right tilt), typically in range [-1.0, 1.0]
 * @param pitch    Pitch control command (positive = nose down), typically in range [-1.0, 1.0]
 * @param yaw      Yaw control command (positive = clockwise), typically in range [-1.0, 1.0]
 * @param out      Output array of 4 floats where the motor outputs will be stored:
 *                   - out[0]: Front Left  (Motor 0)
 *                   - out[1]: Front Right (Motor 1)
 *                   - out[2]: Rear Left   (Motor 2)
 *                   - out[3]: Rear Right  (Motor 3)
 */
void motor_mixer_mix(float throttle, float roll, float pitch, float yaw, float out[NUM_MOTORS]);


/**
* Motor mixer for Quad-X
* Front Right (0)
* Front Left  (1)
* Rear Right  (2)
* Rear Left   (3)
*/
void motor_mixer_quad_x(
    float throttle_norm,       // 0..1 throttle
    float roll_out, float pitch_out, float yaw_out,  // ±PID_LIMIT
    float throttle_idle,  // e.g., 0..1 idle throttle
    float motor_dshot[NUM_MOTORS]     // output throttle per motor
);

#ifdef __cplusplus
}
#endif

#endif // __MOTOR_MIXER_H__
