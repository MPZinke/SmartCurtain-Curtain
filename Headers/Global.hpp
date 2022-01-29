
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Stores info that is included on other dependencies of the Node.ino file.  Breaks values into differnt *
*    namespaces to separate functionalities. Holds C-String custom functions (similar to classic C-String functions),  *
*    Curtain class declaration & other global stuff, Json functions for received message interpretation. JSON          *
*    functions are limited to JSON formats received for this project.                                                  *
*   NOTES:   - With uint8_t return values, 255 (-1) is often used as a "bad value". Therefore, iterations must stop at *
*              less than 255.  Otherwise, the 255th value and -1 will be ambiguous or the final x++ will overflow the  *
*              returned number or a bad number is returned.                                                            *
*   BUGS:                                                                                                              *
*   FUTURE:  - Consider expanding JSON functions to be less exclusive                                                  *
*                                                                                                                      *
***********************************************************************************************************************/


#ifndef __GLOBAL__
#define __GLOBAL__


#include <ArduinoJson.h>
#include <setjmp.h>
#include <stdint.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>


#include "Config.hpp"
#include "Curtain.hpp"


// ——————————————————————————————————————————————————— NAMESPACED ——————————————————————————————————————————————————— //

namespace Global
{
	extern Curtain::Curtain curtain;

	extern WiFiClient client;
	extern WiFiServer server;

	extern jmp_buf jump_buffer;
} // end namespace Global


#endif
