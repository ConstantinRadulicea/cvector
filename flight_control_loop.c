#include "flight_control_loop.h"
#include "geometry3D.h"
#include "geometry2D.h"
#include "string.h"
#include <stdio.h>
#include <time.h>

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

#define RC_INPUT_SAMPLE_RATE_HZ FLIGHT_CONTROLLER_PID_FREQ_HZ
#define RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ 40
#define RC_INPUT_DEADBAND 0.06f

#define THROTTLE_IDLE 0.055f



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


void flight_control_loop_arm_esc(flight_control_loop_t* fcl) {
	fcl->are_esc_armed = 1;
}

void flight_control_loop_disarm_esc(flight_control_loop_t* fcl) {
	fcl->are_esc_armed = 0;
}

int flight_control_loop_are_esc_armed(flight_control_loop_t* fcl) {
	return fcl->are_esc_armed;
}

void flight_control_loop_get_motor_throttle(flight_control_loop_t* fcl, float motors_throttle[NUM_MOTORS]) {
	for (size_t i = 0; i < NUM_MOTORS; i++) {
		motors_throttle[i] = fcl->motor_throttle[i];
	}
}

void flight_control_loop_set_gyro_hook(flight_control_loop_t* fcl, fcl_get_gyro_data_fn hook, void* context) {
	fcl->get_gyro_data = hook;
	fcl->gyro_hook_context = context;
}

void flight_control_loop_set_accel_hook(flight_control_loop_t* fcl, fcl_get_accel_data_fn hook, void* context) {
	fcl->get_accel_data = hook;
	fcl->accel_hook_context = context;
}

void flight_control_loop_set_target_attitude_hook(flight_control_loop_t* fcl, fcl_get_target_attitude_fn hook, void* context) {
	fcl->get_target_attitude = hook;
	fcl->target_attitude_hook_context = context;
}

void flight_control_loop_set_target_throttle_hook(flight_control_loop_t* fcl, fcl_get_target_throttle_fn hook, void* context) {
	fcl->get_target_throttle = hook;
	fcl->target_throttle_hook_context = context;
}

void flight_control_loop_set_write_throttle_hook(flight_control_loop_t* fcl, fcl_write_throttle_to_motors_fn hook, void* context) {
	fcl->write_throttle = hook;
	fcl->write_throttle_hook_context = context;
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

	rc_attitude_control_init_roll(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_ANGLE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

	rc_attitude_control_init_pitch(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_ANGLE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

	rc_attitude_control_init_yaw(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		MAX_RATE,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);

	rc_attitude_control_init_throttle(
		&fcl->rc_attitude_control,
		1,
		RC_INPUT_FILTER_CUTOFF_FREQUENCY_HZ,
		RC_INPUT_DEADBAND,
		1,
		0.0f,
		RC_INPUT_SAMPLE_RATE_HZ
	);
}


void flight_control_loop_tick(flight_control_loop_t* fcl) {
	// RC input
	coord3D target_attitude = {0};
	float target_throttle = 0.0f;

	rc_attitude_control_get_processed(
		&fcl->rc_attitude_control,
		&(target_attitude.x),
		&(target_attitude.y),
		&(target_attitude.z),
		&target_throttle
	);

	// Get estimated attitude and body frame accel/gyro
	coord3D body_frame_accel, body_frame_gyro;
	quaternion body_frame_estimated_q;
	imu_get_estimated_data(&fcl->imu, &body_frame_estimated_q, &body_frame_accel, &body_frame_gyro);

	// Update attitude controller
	attitude_controller_angle_mode_update(
		&fcl->attitude_controller,
		body_frame_estimated_q,
		target_attitude.x, // target roll
		target_attitude.y,  // target pitch
		target_attitude.z   // target yaw rate
	);


	float target_roll_rate = 0.0f;
	float target_pitch_rate = 0.0f;
	float target_yaw_rate = 0.0f;
	attitude_controller_get_calculated_rate(
		&fcl->attitude_controller,
		&target_roll_rate,
		&target_pitch_rate,
		&target_yaw_rate
	);

	float pid_roll_output = 0.0f;
	float pid_pitch_output = 0.0f;
	float pid_yaw_output = 0.0f;
	if (target_throttle > THROTTLE_IDLE)
	{
		rate_controller_update(
			&fcl->rate_controller,
			body_frame_gyro.x,
			body_frame_gyro.y,
			body_frame_gyro.z,
			target_roll_rate,
			target_pitch_rate,
			target_yaw_rate
		);
	}
	else {
		rate_controller_reset(&fcl->rate_controller);
	}

	// Get PID outputs
	rate_controller_get_pid_outputs(
		&fcl->rate_controller,
		&pid_roll_output,
		&pid_pitch_output,
		&pid_yaw_output
	);

	// Mix PID outputs to motor commands
	if (flight_control_loop_are_esc_armed(fcl) != 0)
	{
		motor_mixer_quad_x(
			target_throttle,
			pid_roll_output,
			pid_pitch_output,
			pid_yaw_output,
			THROTTLE_IDLE,
			fcl->motor_throttle
		);
	}
	else {
		for (int i = 0; i < NUM_MOTORS; i++) {
			fcl->motor_throttle[i] = 0.0f;
		}
	}

}


void rc_control_tick(flight_control_loop_t* fcl) {
	// RC input
	coord3D target_attitude = { 0 };
	float target_throttle = 0.0f;

	if (fcl->get_target_attitude) {
		target_attitude = fcl->get_target_attitude(fcl, fcl->target_attitude_hook_context);
	}

	if (fcl->get_target_throttle) {
		target_throttle = fcl->get_target_throttle(fcl, fcl->target_throttle_hook_context);
	}

	rc_attitude_control_update(
		&fcl->rc_attitude_control,
		target_attitude.x,
		target_attitude.y,
		target_attitude.z,
		target_throttle
	);
}

void imu_tick(flight_control_loop_t* fcl) {
	// Read IMU data
	coord3D gyro_data = { 0 };
	coord3D accel_data = { 0 };
	if (fcl->get_gyro_data) {
		gyro_data = fcl->get_gyro_data(fcl, fcl->gyro_hook_context);
	}
	if (fcl->get_accel_data) {
		accel_data = fcl->get_accel_data(fcl, fcl->accel_hook_context);
	}

	imu_update(&fcl->imu, accel_data, gyro_data);
}

void write_to_motors_tick(flight_control_loop_t* fcl) {
	if (fcl->write_throttle) {
		fcl->write_throttle(fcl, fcl->motor_throttle, fcl->write_throttle_hook_context);
	}
}


/*================================================= TEST FUNCTIONS =============================================*/
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
	return (coord3D) { 0.0f, 0.0f, 0.0f };
	return (coord3D) {
		frand_range(-1.0f, 1.0f),
			frand_range(-1.0f, 1.0f),
			frand_range(-1.0f, 1.0f)
	};
}
coord3D get_accel_data() {
	// Placeholder function to simulate acc data retrieval
	return (coord3D) { 1.0f, 0.0f, 0.0f };
	return (coord3D) {
		frand_range(-10.0f, 10.0f),
			frand_range(-10.0f, 10.0f),
			frand_range(-10.0f, 10.0f)
	};
}

coord3D get_target_attitude() {
	// Placeholder function to simulate target attitude retrieval
	return (coord3D) { 0.0f, 0.0f, 0.0f };
	return (coord3D) { 1.0f, -0.5f, -1.0f };
}

float get_target_throttle() {
	// Placeholder function to simulate target throttle retrieval
	return 0.3f;
}

void write_motor_commands(void *gg, float motor_commands[NUM_MOTORS], void *gg2) {
	printf("Motor Commands: FR:%.3f, FL:%.3f, RR:%.3f, RL:%.3f\n", motor_commands[0]*100.0f, motor_commands[1] * 100.0f, motor_commands[2] * 100.0f, motor_commands[3] * 100.0f);
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
		Dshot_value = (uint16_t)(dshot_value + (float)DSHOT_MIN_THROTTLE);
	}
	else {
		Dshot_value = (uint16_t)(CLAMP(dshot_value, DSHOT_MIN_THROTTLE, DSHOT_MAX_THROTTLE));
	}

	return Dshot_value;
}




void flight_control_loop_test() {
	random_seed_once();
	flight_control_loop_t fcl;
	flight_control_loop_init(&fcl);

	flight_control_loop_set_gyro_hook(&fcl, get_gyro_data, NULL);
	flight_control_loop_set_accel_hook(&fcl, get_accel_data, NULL);
	flight_control_loop_set_target_attitude_hook(&fcl, get_target_attitude, NULL);
	flight_control_loop_set_target_throttle_hook(&fcl, get_target_throttle, NULL);
	flight_control_loop_set_write_throttle_hook(&fcl, write_motor_commands, NULL);

	flight_control_loop_arm_esc(&fcl);
	//flight_control_loop_disarm_esc(&fcl);

	while (1) {
		Sleep(1); // Simulate 1 ms delay for 1000 Hz loop
		rc_control_tick(&fcl);
		imu_tick(&fcl);
		flight_control_loop_tick(&fcl);
		write_to_motors_tick(&fcl);
		// Simulate a delay for the control loop frequency
		// In a real system, this would be handled by a timer interrupt or RTOS
	}
}