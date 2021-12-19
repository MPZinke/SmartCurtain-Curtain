
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.06.06                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#ifndef __TRANSMISSION__
#define __TRANSMISSION__


#include "Global.hpp"

#include "Exceptions.hpp"


namespace Transmission
{
	namespace Literal
	{
		namespace HTTP
		{
			// ———— START LINE ———— //
			const char VALID_REQUEST[] = "HTTP/1.1 200 OK";  // start string for valid request from device
			const char INVALID_REQUEST[] = "HTTP/1.1 400 Bad Request";  // start string for invalid request from device
			const char NO_CONTENT_REQUEST[] = "HTTP/1.1 204 No Content";  // start string for no content for request
			const char NOT_FOUND_REQUEST[] = "HTTP/1.1 404 Not Found";  // start string for no content for request
			// —— START LINE::POST —— //
			const char POST_METHOD[] = "POST ";
			const char HTTP_VERSION[] = " HTTP/1.1";

			// ———— HEADERS ———— //
			const char CONTENT_TYPE[] = "Content-Type: application/json";
			const char CONTENT_LENGTH_TAG[] = "Content-Length: ";
			const char HOST_TAG[] = "Host: ";
		}


		namespace JSON
		{
			namespace Key
			{
				const char QUERY_TYPE[] = "query type";

				const char CURTAIN[] = "curtain";
				const char CURTAIN_ID[] = "id";
				const char CURRENT_POS[] = "current position";
				const char LENGTH[] = "length";
				const char CALIBRATE[] = "auto calibrate";
				const char CORRECT[] = "auto correct";
				const char DISCRETE_MOVEMENT[] = "discrete movement";
				const char IS_SMART[] = "is smart";

				const char EVENT[] = "event";
				const char EVENT_ID[] = "id";
				const char EVENT_FORCE[] = "FORCE";
				const char EVENT_POSITION[] = "desired position";
			}


			namespace Value
			{
				const char MOVE[] = "move";  // Query type value for move
				const char RESET[] = "reset";  // Query type value for reset
				const char STATUS[] = "status";  // Query type value for status

				// Used to auto increment the ID for the value
				enum
				{
					MOVE_ID,
					RESET_ID,
					STATUS_ID
				};


				typedef struct
				{
					const uint8_t id;
					const char* value;
				} ValueID;


				const ValueID VALUE_IDS[] =
				{
					{MOVE_ID, MOVE},
					{RESET_ID, RESET},
					{STATUS_ID, STATUS}
				};
			}
		}


		namespace Responses
		{
			const uint8_t INVALID[] = "{\"error\" : \"Package received does not match JSON format\"}";
			const uint8_t VALID[] = "{\"success\":\"Valid JSON received\"}";
		}
	}


	uint8_t id_for_value(const char* value);
	char* http_exception_json(uint16_t error_code, char error_message[]);
	static char* status_json();
	bool skip_header();
	WiFiClient wait_for_request();
	char* read_transmission_data_into_buffer();
	void post_json(char json[], const uint8_t path[]=Config::Transmission::ACTION_COMPLETE_URL);
	void respond_with_json_and_stop(char json[], const char response_type[]=Literal::HTTP::VALID_REQUEST);
	void send_status_and_stop_client();
	void update_hub(byte packet_buffer[]);
} // end namespace Transmission


#endif
