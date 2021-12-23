
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.10                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Curtain.hpp"

#include "../Headers/Global.hpp"
#include "../Headers/C_String.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Transmission.hpp"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //

	Curtain::Curtain(bool initialize)
	{
		using Config::Hardware::CLOSE_ENDSTOP;
		using Config::Hardware::OPEN_ENDSTOP;
		using Config::Hardware::ENCODER;

		_auto_calibrate = CLOSE_ENDSTOP && OPEN_ENDSTOP;
		_auto_correct = CLOSE_ENDSTOP || OPEN_ENDSTOP;
		_discrete_movement = CLOSE_ENDSTOP || OPEN_ENDSTOP;
		_direction = Config::Hardware::DIRECTION_SWITCH;

		_length = Config::Hardware::DEFAULT_LENGTH;
		_position = Movement::current_state() == MIDDLE ? _length / 2 : Movement::current_state() * _length;
	}


	// FREE ME WHEN DONE
	// SUMMARY: Creates a malloced char array the size of the serialized json and writes it.
	// DETAILS: Called when a Curtain object is attempted to be converted to a char*. Converts object to a JsonObject.
	//  Mallocs char* array for c_string. Serializes data to c_string.
	Curtain::operator char*()
	{
		JsonObject curtain_object = (JsonObject)(*this);

		size_t c_string_size = measureJson(curtain_object) + 1;
		char* json_c_string = (char*)malloc(c_string_size);
		serializeJson(curtain_object, json_c_string, c_string_size);

		return json_c_string;
	}


	// SUMMARY: Creates a JsonObject for partially JSONing the Curtain object.
	// DETAILS: Called when a Curtain object is attempted to be converted to a JsonObject. Adds object attributes to
	//  JsonObject and returns it.
	Curtain::operator JsonObject()
	{
		JsonObject curtain_object = JsonObject();

		curtain_object[Transmission::Literal::JSON::Key::CURTAIN_ID] = Config::Curtain::CURTAIN_ID;
		curtain_object[Transmission::Literal::JSON::Key::AUTO_CALIBRATE] = _auto_calibrate;
		curtain_object[Transmission::Literal::JSON::Key::AUTO_CORRECT] = _auto_correct;
		curtain_object[Transmission::Literal::JSON::Key::DIRECTION] = _direction;
		curtain_object[Transmission::Literal::JSON::Key::DISCRETE_MOVEMENT] = _discrete_movement;
		curtain_object[Transmission::Literal::JSON::Key::LENGTH] = _length;
		curtain_object[Transmission::Literal::JSON::Key::CURTAIN_PERCENTAGE] = _percentage;
		curtain_object[Transmission::Literal::JSON::Key::CURTAIN_POSITION] = _position;

		return curtain_object;
	}


	// void Curtain::move()
	// {
		
	// 	if(!curtain.is_smart()) Movement::move(curtain);
	// 	else
	// 	{
	// 		if(!_event.event_moves_to_an_end()) Movement::move(curtain);
	// 		// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
	// 		// ing or closing curtain.
	// 		// Also does not take into account if desire == current.  It can be 'move 0' or ignored by Master.
	// 		else
	// 		{
	// 			if(should_auto_calibrate_across()) _length(Movement::auto_calibrate_to_opposite(inline direction()));
	// 			else if(inline state_of_position() == Curtain::OPEN) Movement::move_until_open(inline direction());
	// 			else Movement::move_until_closed(inline direction());
	// 		}
	// 	}
	// }


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	bool Curtain::auto_calibrate()
	{
		return _auto_calibrate;
	}


	bool Curtain::auto_correct()
	{
		return _auto_correct;
	}


	bool Curtain::direction()
	{
		return _direction;
	}


	bool Curtain::discrete_movement()
	{
		return _discrete_movement;
	}


	uint32_t Curtain::length()
	{
		return _length;
	}


	uint8_t Curtain::percentage()
	{
		return _percentage;
	}


	uint32_t Curtain::position()
	{
		return _position;
	}


	CurtainState state()
	{
		register percentage = _percentage;

		if(!percentage) return CLOSED;
		if(percentage == 100) return OPEN;
		return MIDDLE;
	}


	// ——————————————————————————————————————————————————— SETTER ——————————————————————————————————————————————————— //
	
	void Curtain::auto_calibrate(register bool new_auto_calibrate)
	{
		_auto_calibrate = new_auto_calibrate;
	}


	void Curtain::auto_correct(register bool new_auto_correct)
	{
		_auto_correct = new_auto_correct;
	}


	void Curtain::direction(register bool new_direction)
	{
		_direction = new_direction;
	}


	void Curtain::length(register uint32_t new_length)
	{
		_length = new_length;
	}


	void Curtain::discrete_movement(register bool new_discrete_movement)
	{
		_discrete_movement = new_discrete_movement;
	}


	void Curtain::percentage(register uint8_t new_percentage)
	{
		_percentage = new_percentage;
		_position = ((uint32_t)new_percentage) * _length / 100;
	}


	void Curtain::position(register uint32_t new_position)
	{
		_position = new_position;
		_percentage = new_position * 100 / _length;  // multiply by 100 first to keep as much precision
	}


	// SUMMARY: Update attributes based on key-values of a json.
	// PARAMS: Takes the json document.
	// DETAILS: For every class attribute key that exists in the JSON, the class attribute is updated to the key's
	//  value.
	void Curtain::update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		using namespace Transmission::Literal;
		using Config::Hardware::CLOSE_ENDSTOP;
		using Config::Hardware::OPEN_ENDSTOP;

		JsonObject curtain_object = json_document[JSON::Key::CURTAIN];

		// Restrict auto_calibrate to hardware
		if(curtain_object.containsKey(JSON::Key::AUTO_CALIBRATE) && OPEN_ENDSTOP && CLOSE_ENDSTOP)
		{
			_auto_calibrate = curtain_object[JSON::Key::AUTO_CALIBRATE];
		}

		// Restrict auto_correct to hardware
		if(curtain_object.containsKey(JSON::Key::AUTO_CORRECT) && (OPEN_ENDSTOP || CLOSE_ENDSTOP))
		{
			_auto_correct = curtain_object[JSON::Key::AUTO_CORRECT];
		}

		if(curtain_object.containsKey(JSON::Key::DIRECTION))
		{
			_direction = curtain_object[JSON::Key::DIRECTION];
		}

		if(curtain_object.containsKey(JSON::Key::DISCRETE_MOVEMENT))
		{
			_discrete_movement = curtain_object[JSON::Key::DISCRETE_MOVEMENT];
		}

		if(curtain_object.containsKey(JSON::Key::LENGTH))
		{
			_length = curtain_object[JSON::Key::LENGTH];
		}

		if(curtain_object.containsKey(JSON::Key::CURTAIN_POSITION))
		{
			_position = curtain_object[JSON::Key::CURTAIN_POSITION];
		}
	}


	// ————————————————————————————————————————————— CLASS::SETTERS: DATA —————————————————————————————————————————————

	// // Corrects position for DB unknowns relative to sensors.
	// // Sets self::_position to match open/closed if applicable.
	// void Curtain::set_position_if_does_not_match_sensors()
	// {
	// 	if(Movement::is_closed() && !is_approximate_position(_position, 0))
	// 		_position = 0;
	// 	else if(Movement::is_open() && !is_approximate_position(_position, _length))
	// 		_position = _length;
	// }


	// // ASSUMES _desired_position WAS REACHED IF NOT AT AN END. COULD BE WRONG.
	// // Sets the location of the curtain based on GPIO if possible, other wise desired location.
	// void Curtain::set_location()
	// {
	// 	if(!_is_smart)
	// 	{
	// 		Global::current_position = _desired_position;
	// 		_position = _desired_position;  // curtain isn't that smart, so guess where it is
	// 	}
	// 	else
	// 	{
	// 		if(Movement::is_open()) _position = _length;
	// 		else if(Movement::is_closed()) _position = 0;
	// 		else _position = _desired_position;  // curtain isn't that smart, so guess where it is
	// 		Global::current_position = _position;
	// 	}
	// }


	// ————————————————————————————————————————————————— CLASS::WRITE —————————————————————————————————————————————————

	// void Curtain::send_hub_serialized_info()
	// {
	// 	char* serialized_data_c_str = serialized_data();
	// 	Transmission::update_hub((byte*)serialized_data_c_str);
	// 	delete[] serialized_data_c_str;
	// }

}