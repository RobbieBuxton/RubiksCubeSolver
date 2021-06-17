#include "main.h"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Rubik's cube solver");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
 #define TIME 500
 #define MAX_SPEED 127

typedef enum {
	RED = 11,    //0
	WHITE = 12,  //1
	GREEN = 13,  //2
	YELLOW = 14, //3
	BLUE = 15,   //4
	ORANGE = 16  //5
} MotorColour;

int get_rotate_amount(int colour) {
	switch(colour){
		case RED:
			return 225;
		case WHITE:
			return 230;
		case GREEN:
			return 225;
		case YELLOW:
			return 225;
		case BLUE:
			return 230;
		case ORANGE:
			return 227;
		default:
			return 225;
	}

}

void opcontrol() {
	//From one of the tests done solving the cube using IDA*
	int scramble[30][2] {{1,2},
											 {2,0},
											 {2,0},
											 {0,1},
											 {2,4},
											 {2,5},
											 {1,3},
											 {2,5},
											 {1,4},
											 {1,1},
											 {1,2},
											 {0,4},
											 {0,3},
											 //solve
											 {2,3},
											 {1,2},
											 {2,4},
											 {1,1},
											 {1,4},
											 {0,5},
											 {1,3},
											 {0,5},
											 {0,4},
											 {2,1},
											 {1,0},
											 {1,2}};



	pros::c::motor_set_brake_mode(11,MOTOR_BRAKE_HOLD);
	pros::c::motor_set_brake_mode(12,MOTOR_BRAKE_HOLD);
	pros::c::motor_set_brake_mode(13,MOTOR_BRAKE_HOLD);
	pros::c::motor_set_brake_mode(14,MOTOR_BRAKE_HOLD);
	pros::c::motor_set_brake_mode(15,MOTOR_BRAKE_HOLD);
	pros::c::motor_set_brake_mode(16,MOTOR_BRAKE_HOLD);


	pros::delay(3000);
	int accumulator[6] = {0,0,0,0,0,0};
	int colour;
	for (int i = 0; i < 0; i++) {
		accumulator[scramble[i][1]] += (scramble[i][0] + 1);
		colour = scramble[i][1]+11;
		pros::c::motor_move_absolute(colour,
			                           (accumulator[scramble[i][1]] % 4) *
																 get_rotate_amount(colour)
																 ,MAX_SPEED/2);
		pros::delay(TIME*2);
	}
	pros::delay(3000);
}
