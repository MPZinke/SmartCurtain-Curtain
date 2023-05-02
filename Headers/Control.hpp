
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2022.07.12                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/



#include <ArduinoJson.h>


namespace Control
{
	void loop();
	void process_message(int);

	void case_move(int message_size);
	void case_status();
	void case_update_curtain(int message_size);
}
