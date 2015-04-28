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

using namespace std;

enum possible_commands {JOG_Z = 0, 		JOG_Y, 		JOG_X, 	 	JOG_XY,
							     	 SET_PW_Z, 	 SET_PW_Y, 	  SET_PW_X,
							     	 	START, 		PAUSE, 		CANCEL
						};

class CommandProcessor
{
public:
	CommandProcessor();

	possible_commands selected_command;
	int input_command(alt_u8 command, string payload);
};


#endif /* COMMAND_PROCESSOR_HPP_ */
