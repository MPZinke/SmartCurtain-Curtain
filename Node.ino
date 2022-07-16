
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Ethernet connection version of smart curtain. This is primarily desired for a Hub-Node model, where   *
*       The Hub schedules events, and the node (this code) queries and activates those events. On event, curtain is    *
*       driven to location, where it updates the Hub. The primary communication is JSON through HTTP POST requests. If *
*       auto_correct option is set, the Hub will attempt to correct the curtain's location, assuming length,           *
*       direction, etc are correct. If the auto_calibrate flag is set and the curtain movement spans the entire rod,   *
*       the curtain will count the total steps taken (±1) and use that as the new length. Information is updated after *
*       each cycle by posting a JSON to the Hub.                                                                       *
*       **Main arduino file**                                                                                          *
*      CONVENTION: Position 0 is CLOSED                                                                                *
*   BUGS:   - If direction option is wrong, gears will be ground :)                                                    *
*           - If the curtain's (driver) step length is > (4294967295 - allowable_difference), curtain may assume       *
*             open position = close position.                                                                          *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*           - Figure out a solution for needing Hub to specify length of curtain.                                      *
*                                                                                                                      *
***********************************************************************************************************************/


#include <ArduinoJson.h>
#include <esp_wifi.h>
#include <SPI.h>


#include "Headers/Config.hpp"
#include "Headers/Global.hpp"

#include "Headers/C_String.hpp"
#include "Headers/Curtain.hpp"
#include "Headers/Event.hpp"
#include "Headers/Exceptions.hpp"
#include "Headers/Hardware.hpp"
#include "Headers/Movement.hpp"
#include "Headers/Request.hpp"


using namespace Exceptions;


void case_default(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);


void setup()
{
	// ———— GPIO SETUP ————
	pinMode(Config::Hardware::CLOSE_PIN, INPUT);  // now analog, technically do not need
	pinMode(Config::Hardware::OPEN_PIN, INPUT);  // now analog, technically do not need

	pinMode(Config::Hardware::DIRECTION_PIN, OUTPUT);
	pinMode(Config::Hardware::ENABLE_PIN, OUTPUT);
	pinMode(Config::Hardware::PULSE_PIN, OUTPUT);

	Hardware::disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
	// wifi setup
	WiFi.mode(WIFI_STA);
	esp_wifi_set_mac(WIFI_IF_STA, Config::Network::MAC_ADDRESS);
	WiFi.begin(Config::Network::SSID, Config::Network::PASSWORD);
	while(WiFi.status() != WL_CONNECTED) delay(500);

	Global::server.begin();
}


void loop()
{
	Hardware::disable_motor();  // don't burn up the motor
		
	StaticJsonDocument<JSON_BUFFER_SIZE> json_document = Request::decode_json();
	if(!Global::exception)
	{
		// If curtain information, update Global::curtain information
		if(json_document.containsKey(Request::Literal::JSON::Key::CURTAIN))
		{
			Global::curtain.update(json_document);
		}

		// Call action for QUERY_TYPE
		switch(Request::id_for_query_type(json_document))
		{
			// Update information about curtain
			case Request::Literal::JSON::Value::UPDATE_ID:
			{
				case_update(json_document);
				// Fall through to STATUS_ID
			}

			// Return information about Curtain
			case Request::Literal::JSON::Value::STATUS_ID:
			{
				Request::send_status_and_stop_client();
				break;
			}

			// Move to position
			case Request::Literal::JSON::Value::MOVE_ID:
			{
				case_move(json_document);
				break;
			}

			default:
			{
				case_default(json_document);
			}
		}
	}

	if(Global::exception)
	{
		Global::exception->send();
	}

	delay(250);
}


// ——————————————————————————————————————————————————— CASES  ——————————————————————————————————————————————————— //

void case_default(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
{
	String message = String("Unknown ") + Request::Literal::JSON::Key::QUERY_TYPE + "'"
	  + (const char*)json_document[Request::Literal::JSON::Key::QUERY_TYPE] + "'";
	new NOT_FOUND_404_Exception(__LINE__, __FILE__, message);
}

void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
{
	if(!json_document.containsKey(Request::Literal::JSON::Key::EVENT))
	{
		String error_message = String("Missing key: '") + Request::Literal::JSON::Key::EVENT + "' for QUERY_TYPE: '"
		  + Request::Literal::JSON::Value::MOVE + "'";
		new NOT_FOUND_404_Exception(__LINE__, __FILE__, error_message);
		return;
	}

	// TODO: If !event.is_activated(): Exception
	Request::respond_with_json_and_stop(Request::Literal::Responses::VALID);
	JsonObject event_object = json_document[Request::Literal::JSON::Key::EVENT];
	Global::event = Event::Event(event_object);

	Request::deactivate_curtain();
}


void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
{
	using namespace Request::Literal::JSON;

	if(!json_document.containsKey(Key::HUB_IP))
	{
		uint8_t octets[4];
		C_String::IP_address_octets(json_document[Key::HUB_IP], octets);
		Global::client_IP = IPAddress(octets[0], octets[1], octets[2], octets[3]);
	}
}
