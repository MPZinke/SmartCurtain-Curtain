
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
#include <soc/rtc_wdt.h>
#include <SPI.h>
#include <WiFi.h>


#include "Headers/Config.hpp"
#include "Headers/Global.hpp"

#include "Headers/C_String.hpp"
#include "Headers/Curtain.hpp"
#include "Headers/Event.hpp"
#include "Headers/Exceptions.hpp"
#include "Headers/Hardware.hpp"
#include "Headers/Movement.hpp"
#include "Headers/Message.hpp"
#include "Headers/Processor.hpp"


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
	while(WiFi.status() != WL_CONNECTED)
	{
		delay(500);
	}

	while(!Global::mqtt_client.connect(Config::Network::BROKER_DOMAIN, Config::Network::BROKER_PORT))
	{
		delay(500);
	}

	{  // for namespacing
		using namespace Message::Literal::MQTT;
		Global::mqtt_client.onMessage(Processor::process_message);
		Global::mqtt_client.subscribe(ALL_CURTAINS);
		Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+MOVE_SUFFIX);
		Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+STATUS_SUFFIX);
		Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+UPDATE_SUFFIX);
	}


	xTaskCreatePinnedToCore((TaskFunction_t)Processor::loop, "MQTT", 10000, NULL, 2, NULL, 0);
	xTaskCreatePinnedToCore((TaskFunction_t)Movement::movement_loop, "Movement", 10000, NULL, 1, NULL, 1);

	// Prevent infinite loop detection
	rtc_wdt_protect_off();
	rtc_wdt_disable();
	disableCore0WDT();
	disableLoopWDT();
}


void loop()
{}
