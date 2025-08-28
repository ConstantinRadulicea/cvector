#ifndef FLIGHT_LOOP_CONTROL_H
#define FLIGHT_LOOP_CONTROL_H

#include "imu.h"
#include "attitude_controller.h"
#include "rate_controller.h"
#include "motor_mixer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct flight_control_loop_s{
	imu_t imu;
	rate_controller_t rate_controller;
	attitude_controller_t attitude_controller;
	float motor_throttle[NUM_MOTORS];
	uint16_t motor_raw_command[NUM_MOTORS];
} flight_control_loop_t;

void flight_control_loop_init(flight_control_loop_t* fcl);
void flight_control_loop_tick(flight_control_loop_t* fcl);
void flight_control_loop_test();

#ifdef __cplusplus
}
#endif

#endif // !FLIGHT_LOOP_CONTROL_H
