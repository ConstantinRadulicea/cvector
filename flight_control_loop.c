#include "flight_control_loop.h"
#include "geometry3D.h"
#include "geometry2D.h"
#include "string.h"
#include <stdio.h>

#define FLIGHT_CONTROLLER_PID_FREQ_HZ 1000

#define ACCELEROMETER_FILTER_CUTOFF_FREQ_HZ 10
#define ACCELEROMETER_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ

#define GYRO_FILTER_CUTOFF_FREQ_HZ 80
#define GYRO_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ


#define CONTROLLER_PID_KP (radians(1.0f) * 50.0f)
#define CONTROLLER_PID_KI (radians(1.0f) * 70.0f)
#define CONTROLLER_PID_KD (radians(1.0f) * 20.0f)
#define CONTROLLER_MAX_INTEGRAL_LIMIT (radians(1.0f) * 200.0f)
#define CONTROLLER_PID_KFF (radians(1.0f) * 80.0f)

#define CONTROLLER_YAW_PID_KP (radians(1.0f) * 45.0f)
#define CONTROLLER_YAW_PID_KI (radians(1.0f) * 70.0f)
#define CONTROLLER_YAW_PID_KD (radians(1.0f) * 0.0f)
#define CONTROLLER_YAW_MAX_INTEGRAL_LIMIT (CONTROLLER_MAX_INTEGRAL_LIMIT)
#define CONTROLLER_YAW_PID_KFF (radians(1.0f) * 90.0f)

#define D_TERM_PID_FILTER_CUTOFF_FREQ_HZ 20
#define FF_TERM_PID_FILTER_CUTOFF_FREQ_HZ 40

#define RATE_GAIN (radians(1.0f) * 50.0f)
#define MAX_ANGLE radians(50.0f)
#define MAX_RATE radians(600.0f)



#define G_CONSTANT 9.80665f
#define G2MS2(g) ((g) * G_CONSTANT)

static float accelerometer_bias[3] = { G2MS2(2.857746f), G2MS2(-2.372569f), G2MS2(-4.517941f)};
static float accelerometer_A_1[3][3] = {
		{0.996747f, 0.000551f, 0.002421f},
		{0.000551f, 0.998310f, 0.002869f},
		{0.002421f, 0.002869f, 0.989385f}
};

static float gyro_bias[3] = { radians(-2.481623f + 0.2f), radians(2.152133f - 0.35f), radians(-0.212093f - 0.28f) };

static quaternion ground_default_position_q = { .w = 1.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f };


#include <stdlib.h>   // rand, srand
#include <time.h>     // time (for seeding once)

static inline float frand_range(float lo, float hi) {
	return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
}

static void random_seed_once(void) {
	static int seeded = 0;
	if (!seeded) {
		srand((unsigned)time(NULL));   // or use HAL_GetTick() on embedded
		seeded = 1;
	}
}

coord3D get_gyro_data() {
	// Placeholder function to simulate gyro data retrieval
	return (coord3D) {
		frand_range(-1.0f, 1.0f),
			frand_range(-1.0f, 1.0f),
			frand_range(-1.0f, 1.0f)
	};
}
coord3D get_accel_data() {
	// Placeholder function to simulate acc data retrieval
	return (coord3D) {
		frand_range(-10.0f, 10.0f),
			frand_range(-10.0f, 10.0f),
			frand_range(-10.0f, 10.0f)
	};
}

coord3D get_target_attitude() {
	// Placeholder function to simulate target attitude retrieval
	return (coord3D) { 0.0f, 0.0f, 0.0f };
}

float get_target_throttle() {
	// Placeholder function to simulate target throttle retrieval
	return 0.0f;
}

void write_motor_commands(uint16_t motor_commands[NUM_MOTORS]) {
	printf("Motor Commands: FR:%d, FL:%d, RR:%d, RL:%d\n", motor_commands[0], motor_commands[1], motor_commands[2], motor_commands[3]);
	// Placeholder function to simulate writing motor commands
}


#define DSHOT_MIN_THROTTLE      48
#define DSHOT_MAX_THROTTLE     	2047
#define DSHOT_RANGE 			(DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)
uint16_t throttle2Dshot_value(float throttle) {
	uint16_t Dshot_value;

	Dshot_value = ((uint16_t)(((float)(DSHOT_RANGE + 1)) * throttle)) + (DSHOT_MIN_THROTTLE - 1);
	if (Dshot_value < (uint16_t)DSHOT_MIN_THROTTLE) {
		Dshot_value = (uint16_t)0;
	}
	else {
		Dshot_value = (uint16_t)(CLAMP(Dshot_value, DSHOT_MIN_THROTTLE, DSHOT_MAX_THROTTLE));
	}

	return Dshot_value;
}


static uint16_t validate_dshot_value(float dshot_value) {
	uint16_t Dshot_value;

	if (dshot_value < 1.0f) {
		Dshot_value = (uint16_t)0;
	}
	else if (dshot_value < (uint16_t)DSHOT_MIN_THROTTLE) {
		Dshot_value = (uint16_t) (dshot_value + (float)DSHOT_MIN_THROTTLE);
	}
	else {
		Dshot_value = (uint16_t)(CLAMP(dshot_value, DSHOT_MIN_THROTTLE, DSHOT_MAX_THROTTLE));
	}

	return Dshot_value;
}


void flight_control_loop_init(flight_control_loop_t *fcl) {
	memset(fcl, 0, sizeof(flight_control_loop_t));

	// Initialize IMU and attitude controller here (not shown)
	imu_init(
		&fcl->imu,
		ACCELEROMETER_FILTER_CUTOFF_FREQ_HZ,
		GYRO_FILTER_CUTOFF_FREQ_HZ,
		ACCELEROMETER_SAMPLE_RATE_HZ,
		GYRO_SAMPLE_RATE_HZ,
		FLIGHT_CONTROLLER_PID_FREQ_HZ
	);

	imu_set_accel_bias(&fcl->imu, (coord3D) { accelerometer_bias[0], accelerometer_bias[1], accelerometer_bias[2] }, accelerometer_A_1);
	imu_set_gyro_bias(&fcl->imu, (coord3D) { gyro_bias[0], gyro_bias[1], gyro_bias[2] });
	imu_set_leveled_attitude(&fcl->imu, ground_default_position_q);

	attitude_controller_init(
		&fcl->attitude_controller,
		RATE_GAIN,
		MAX_ANGLE,
		MAX_RATE
	);

	rate_controller_init(
		&fcl->rate_controller,
		FLIGHT_CONTROLLER_PID_FREQ_HZ,
		D_TERM_PID_FILTER_CUTOFF_FREQ_HZ,
		FF_TERM_PID_FILTER_CUTOFF_FREQ_HZ
	);

	rate_controller_init_roll_pid(
		&fcl->rate_controller,
		CONTROLLER_PID_KP,
		CONTROLLER_PID_KI,
		CONTROLLER_PID_KD,
		CONTROLLER_MAX_INTEGRAL_LIMIT,
		CONTROLLER_PID_KFF
	);

	rate_controller_init_pitch_pid(
		&fcl->rate_controller,
		CONTROLLER_PID_KP,
		CONTROLLER_PID_KI,
		CONTROLLER_PID_KD,
		CONTROLLER_MAX_INTEGRAL_LIMIT,
		CONTROLLER_PID_KFF
	);

	rate_controller_init_yaw_pid(
		&fcl->rate_controller,
		CONTROLLER_YAW_PID_KP,
		CONTROLLER_YAW_PID_KI,
		CONTROLLER_YAW_PID_KD,
		CONTROLLER_YAW_MAX_INTEGRAL_LIMIT,
		CONTROLLER_YAW_PID_KFF
	);
}


void flight_control_loop_tick(flight_control_loop_t* fcl) {
	// Read IMU data
	coord3D gyro_data = get_gyro_data();
	coord3D accel_data = get_accel_data();
	imu_update(&fcl->imu, accel_data, gyro_data);

	// Get estimated attitude and body frame accel/gyro
	coord3D body_frame_accel, body_frame_gyro;
	quaternion body_frame_estimated_q;
	imu_get_estimated_data(&fcl->imu, &body_frame_estimated_q, &body_frame_accel, &body_frame_gyro);

	// Update attitude controller
	coord3D target_attitude = get_target_attitude();
	attitude_controller_angle_mode_update(
		&fcl->attitude_controller,
		body_frame_estimated_q,
		target_attitude.x, // target roll
		target_attitude.y,  // target pitch
		target_attitude.z   // target yaw rate
	);

	float target_roll_rate, target_pitch_rate, target_yaw_rate;
	attitude_controller_get_calculated_rate(
		&fcl->attitude_controller,
		&target_roll_rate,
		&target_pitch_rate,
		&target_yaw_rate
	);

	rate_controller_update(
		&fcl->rate_controller,
		body_frame_gyro.x,
		body_frame_gyro.y,
		body_frame_gyro.z,
		target_roll_rate,
		target_pitch_rate,
		target_yaw_rate
	);

	// Get PID outputs
	float pid_roll_output, pid_pitch_output, pid_yaw_output;
	rate_controller_get_pid_outputs(
		&fcl->rate_controller,
		&pid_roll_output,
		&pid_pitch_output,
		&pid_yaw_output
	);

	// Mix PID outputs to motor commands
	float target_throttle = get_target_throttle();
	target_throttle = validate_dshot_value(target_throttle * (float)DSHOT_MAX_THROTTLE);
	motor_mixer_quad_x(
		target_throttle,
		pid_roll_output,
		pid_pitch_output,
		pid_yaw_output,
		0.0f,
		fcl->motor_throttle
	);

	// Constrain motor commands and convert to raw values
	for (int i = 0; i < NUM_MOTORS; i++) {
		fcl->motor_raw_command[i] = throttle2Dshot_value(fcl->motor_throttle[i]);
	}
	write_motor_commands(fcl->motor_raw_command);
}

void flight_control_loop_test() {
	random_seed_once();
	flight_control_loop_t fcl;
	flight_control_loop_init(&fcl);

	while (1) {
		Sleep(1); // Simulate 1 ms delay for 1000 Hz loop
		flight_control_loop_tick(&fcl);
		// Simulate a delay for the control loop frequency
		// In a real system, this would be handled by a timer interrupt or RTOS
	}
}