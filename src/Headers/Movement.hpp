
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.28                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Handware control part of the code. The trickiest part of this page is the XOR—direction and ON/OFF    *
*       are depenent on physical setup of the motor, and the High/Low of the stepper driver.                           *
*   CONVENTION: - OPEN = ON, CLOSE = OFF                                                                               *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include "Global.hpp"
#include "Curtain.hpp"


namespace Movement
{
	void disable_motor();
	void enable_motor();
	void set_direction(bool direction_current);
	bool is_closed();
	bool is_open();
	Curtain::CurtainState state();
	uint32_t steps_for_direction(bool direction, uint32_t current_position, uint32_t desired_position);
	bool move(Curtain::Curtain& curtain);
	void move_until_state_reached(bool(*state_function)());
	uint32_t move_and_count_until_state_reached(bool(*state_function)());
	bool sensor_triggered_moving_steps(register uint32_t steps, bool(*state_function)());
	void move_until_closed();
	void move_until_open();
	uint32_t calibrate_to_opposite(bool curtain_direction);
	void move_motor_step_count(register uint32_t steps);



	// ————————————————————————————————————————————————— GPIO: GLOBAL —————————————————————————————————————————————————

	// ———— SUGAR ————
	const bool ON = HIGH ^ Config::Hardware::SWITCH;  // the "ON"/"ACTIVATE" state for the device
	const bool OFF = !ON;  // the "OFF"/"DEACTIVATE" state for the device
	const bool CLOSE = OFF;  // solidify convention
	const bool OPEN = !CLOSE;  // solidify convention

}  // end namespace GPIO
