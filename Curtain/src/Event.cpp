
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Event objects are supposed to be relatively dumb—they are supposed to know where they want to go      *
*    generally speaking and that is it. How an event acheives its movement means nothing to it.                        *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Event.hpp"


#include "../Headers/Global.hpp"

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/MQTT.hpp"
#include "../Headers/StaticString.hpp"


#define PERCENTAGE "percentage"


namespace Event
{
	using namespace Curtain::CurtainStates;


	bool validate(DeserializedJSON::DeserializedJSON& event_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		// Validate structure
		if(event_json.containsKey(PERCENTAGE) && !event_json[PERCENTAGE].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Event object requires key '"PERCENTAGE"' of type 'int'");
			return false;
		}

		return true;
	}

	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //


	Event::Event(DeserializedJSON::DeserializedJSON& event_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_percentage = event_json[PERCENTAGE];
	}


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	uint8_t Event::percentage()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _percentage;
	}

	// ——————————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————————— //

	// SUMMARY: Creates a String JSON for the Event.
	// DETAILS: Called when a Curtain object is attempted to be converted to a char*. Converts object to a JsonObject.
	//  Mallocs char* array for c_string. Serializes data to c_string.
	Event::operator StaticString<JSON_BUFFER_SIZE>()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject event_object = json_document.to<JsonObject>();

		event_object[PERCENTAGE] = _percentage;

		return StaticString<JSON_BUFFER_SIZE>(event_object);
	}


	// ——————————————————————————————————————————————————— SETTER ——————————————————————————————————————————————————— //

	void Event::percentage(uint8_t percentage)
	{
		_percentage = percentage;
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //

	CurtainState Event::direction()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(_percentage < Global::curtain.percentage())
		{
			return CLOSE;
		}

		return OPEN;
	}


	bool Event::event_moves_to_an_end()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _percentage == 0 || _percentage == 100;
	}


	uint32_t Event::steps()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		uint8_t percentage_delta = Global::curtain.percentage() - _percentage;
		if(percentage_delta < 0)
		{
			percentage_delta *= -1;
		}

		return percentage_delta * Global::curtain._length / 100;
	}
}  // end namespace Event
