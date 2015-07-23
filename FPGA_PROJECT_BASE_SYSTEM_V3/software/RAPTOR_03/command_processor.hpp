/////////////////////////////////////////////////////////////////////////////
/// @file   command_processor.hpp
///
/// @date	27 avr. 2015 20:33:57
///
/// @author	Michael House
///
/// @brief
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_PROCESSOR_HPP_
#define COMMAND_PROCESSOR_HPP_

#include <string>
#include <iostream>

#include "types.hpp"
#include "cncmachine.hpp"



struct cnc_stepdir
{
    union
    {
        alt_u32 ULONG;
        struct
        {
            alt_u32 step : 31;
            alt_u32 dir : 1;
        }bits;
    }data;
};

enum possible_commands {JOG_Z = 0,      JOG_Y,      JOG_X,          JOG_XY,
                                     SET_PW_Z,   SET_PW_Y,        SET_PW_X,
                                  START_ROUTE,      PAUSE,          CANCEL,
                                         G_XY,       FEED,     ERASE_COORD,
                                    SET_LAYER,  SET_ACCEL, SET_ROUTE_STATE,
                                          G_Z,
                        };
class CommandProcessor : private CncMachine
{
public:
	CommandProcessor();

	int input_command(alt_u8 command, string payload);

private:
	alt_u32 get_long_from_string(string in_string, alt_u8 index);
	alt_u16 get_word_from_string(string in_string, alt_u8 index);
	alt_u8 get_byte_from_string(string in_string, alt_u8 index);
	list<string> get_fields(string in_string);
	cnc_stepdir get_step_and_dir(string payload);
	cnc_stepdir get_step_and_dir(alt_u32 value);

	void jog_z(string payload);
	void jog_y(string payload);
	void jog_x(string payload);
	void jog_xy(string payload);
	void set_pw_z(string payload);
	void set_pw_y(string payload);
	void set_pw_x(string payload);
	void set_pw_feed(string payload);
	void set_coordinate(string payload);
	void set_acceleration(string payload);
	void set_layer(string payload);
	void set_router_state(string payload);
};


#endif /* COMMAND_PROCESSOR_HPP_ */
