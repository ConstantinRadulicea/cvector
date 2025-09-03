#ifndef FLIGHT_LOOP_CONTROL_H
#define FLIGHT_LOOP_CONTROL_H

#include "imu.h"
#include "attitude_controller.h"
#include "rate_controller.h"
#include "motor_mixer.h"
#include "rc_control.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef coord3D(*fcl_get_gyro_data_fn)(struct flight_control_loop_s* fcl, void* context);
typedef coord3D(*fcl_get_accel_data_fn)(struct flight_control_loop_s* fcl, void* context);
typedef coord3D(*fcl_get_target_attitude_fn)(struct flight_control_loop_s* fcl, void* context);
typedef float (*fcl_get_target_throttle_fn)(struct flight_control_loop_s* fcl, void* context);
typedef void (*fcl_write_throttle_to_motors_fn)(struct flight_control_loop_s* fcl, float motor_throttle[NUM_MOTORS], void* context);


typedef struct flight_control_loop_s{
	imu_t imu;
	int are_esc_armed;
	rate_controller_t rate_controller;
	attitude_controller_t attitude_controller;
	rc_attitude_control_t rc_attitude_control;
	float motor_throttle[NUM_MOTORS];

	fcl_get_gyro_data_fn get_gyro_data;
	fcl_get_accel_data_fn get_accel_data;
	fcl_get_target_attitude_fn get_target_attitude;
	fcl_get_target_throttle_fn get_target_throttle;
	fcl_write_throttle_to_motors_fn write_throttle;

	void* gyro_hook_context;
	void* accel_hook_context;
	void* target_attitude_hook_context;
	void* target_throttle_hook_context;
	void* write_throttle_hook_context;

} flight_control_loop_t;

void flight_control_loop_arm_esc(flight_control_loop_t* fcl);
void flight_control_loop_disarm_esc(flight_control_loop_t* fcl);
int flight_control_loop_are_esc_armed(flight_control_loop_t* fcl);
void flight_control_loop_get_motor_throttle(flight_control_loop_t* fcl, float motors_throttle[NUM_MOTORS]);
void write_to_motors_tick(flight_control_loop_t* fcl);

void flight_control_loop_init(flight_control_loop_t* fcl);
void flight_control_loop_tick(flight_control_loop_t* fcl);
void flight_control_loop_test();

void flight_control_loop_set_gyro_hook(flight_control_loop_t* fcl, fcl_get_gyro_data_fn hook, void* context);
void flight_control_loop_set_accel_hook(flight_control_loop_t* fcl, fcl_get_accel_data_fn hook, void* context);
void flight_control_loop_set_target_attitude_hook(flight_control_loop_t* fcl, fcl_get_target_attitude_fn hook, void* context);
void flight_control_loop_set_target_throttle_hook(flight_control_loop_t* fcl, fcl_get_target_throttle_fn hook, void* context);
void flight_control_loop_set_write_throttle_hook(flight_control_loop_t* fcl, fcl_write_throttle_to_motors_fn hook, void* context);

#ifdef __cplusplus
}
#endif

#endif // !FLIGHT_LOOP_CONTROL_H
