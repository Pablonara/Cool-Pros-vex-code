#include "main.h" //definitions
#include "autoSelect/selection.h"
Motor rightwheel(19);
Motor leftwheel(20);
Motor elevate1(6);
Motor elevate2(7);//reversed direction
Motor intake(13);
Motor flywheel(12);
Motor flywheel1(11); // reversed direction
Controller master(E_CONTROLLER_MASTER);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.

void on_center_button() { // autonnomous selector test. TBA //added in void autonomous()
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
} 
 */
/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// pros::lcd::initialize();
	// pros::lcd::set_text(2, "test");

	// pros::lcd::register_btn1_cb(on_center_button);
	selector::init();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */

// BRAKE MODE HOLD DOES NOT WORK IN DISABLED, VEXOS OVERRIDES
int test1 = 0;
void disabled() {
	elevate1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); // change brake mode to hold mode: dynamic braking with a PID loop to hold the motor in place so when you release elevate button it stayas in place
	elevate2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	elevate1 = 30;
	elevate2 = -30;
	pros::lcd::set_text(2, "test");
	pros::lcd::set_text(3, std::to_string(test1));
	test1++;
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {	
	elevate1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); // change brake mode to hold mode: dynamic braking with a PID loop to hold the motor in place so when you release elevate button it stayas in place
	elevate2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);}

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

void autonomous() {
	/**
	change names in autoSelect/selection.h for gui names
	selector::auton == 1 : Red Front
	selector::auton == 2 : Red Back
	selector::auton == 3 : Do Nothing
	selector::auton == -1 : Blue Front
	selector::auton == -2 : Blue Back 
	selector::auton == -3 : Do Nothing
	selector::auton == 0 : Skills 
	**/

	using namespace okapi; // use okapi namespace, custom library located at projectfolder/include/okapi/api.hpp. Uncomment the #include okapi.hpp in main.h config file
	std::shared_ptr<ChassisController> drive =
    ChassisControllerBuilder() // chassiscontroller definition, prebuilt library. Less accurate than one with 1 or 2 tracking wheeel but we don't really have time for a good code version
        .withMotors(-20, 19) // motor ports
        .withDimensions(AbstractMotor::gearset::green, {{4_in, 12.5_in}, imev5GreenTPR} )// Green gearset, 4 in wheel diam, 12.5 in wheel track (4pi)
        .build();
	drive->setMaxVelocity(50); // set max velocity to 50 rpm (25% power on green gearset for more accurate pid)
	
	if(selector::auton == 1){ //run auton for Front Red 
	drive->moveDistance(22_in); // move 22 inches forward; float value
	drive->turnAngle(90_deg);
	drive->moveDistance(8_in);
	}
	// else if(selector::auton == 2){ //run auton for Back Red }
	else if(selector::auton == -1){ //run auton for Front Blue 
	drive->moveDistance(22_in); // move 22 inches forward
	drive->turnAngle(-90_deg);
	drive->moveDistance(8_in);
	}
	// else if(selector::auton == -2){ //run auton for Back Blue; commented out because throws error if else block is empty }
	// else if(selector::auton == 0){ //run auton for Skills }
	else{ //do nothing }
}
}
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
void opcontrol() { // driver control period
	using namespace pros; // using pros namespace
	double rightx;
    double righty; 
	double lefty;
    // stick drift calibration, runs on assumption that the stick is at 0,0 on program start. Not in use, no need currently
    double rxcalibrate = master.get_analog(ANALOG_RIGHT_Y);
    double rycalibrate = master.get_analog(ANALOG_RIGHT_X);
	rightx -= rxcalibrate;
    righty -= rycalibrate;
	pros::Controller master(pros::E_CONTROLLER_MASTER); // define master controller as master, since pros supports multiple controller bound to one robot
	elevate1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); // change brake mode to hold mode: dynamic braking with a PID loop to hold the motor in place so when you release elevate button it stayas in place
	elevate2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	flywheel.set_brake_mode(pros::E_MOTOR_BRAKE_COAST); //lower chance of motor burning out with coasting flywheel
	flywheel1.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);  
	while (true) {
		rightx = master.get_analog(ANALOG_LEFT_X);
		righty = master.get_analog(ANALOG_LEFT_Y);
		lefty = master.get_analog(ANALOG_RIGHT_Y);

		// pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		//                  (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		//                  (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0); // debug functions
		
		rightwheel = righty +rightx; // arcade drivemode, reverse one motor direction so it drives normally
		leftwheel = rightx-righty;
		if (lefty != 0){
			flywheel.move_velocity(-lefty);
    		flywheel1.move_velocity(lefty);
		}
		if(master.get_digital(DIGITAL_UP)){
			elevate1 = 127;
			elevate2 = -127; 
		}
		else if(master.get_digital(DIGITAL_DOWN)){
			elevate1 = -127;
			elevate2 = 127;
		}
		else{
			elevate1 = 0;
			elevate2 = 0;
		}
		if(master.get_digital(DIGITAL_L1)){
			intake = 127;
		}
		else if(master.get_digital(DIGITAL_L2)){
			intake = -127;
		}
		else{
			intake = 0;
		}
		if(master.get_digital(DIGITAL_R1)){
			flywheel = -127;
			flywheel1 = 127;
		}
		else if(master.get_digital(DIGITAL_R2)){
			flywheel = 127;
			flywheel1 = -127;
		}
		else{
			flywheel = 0;
			flywheel1 = 0;
		}
		pros::delay(20);
	}
}
	