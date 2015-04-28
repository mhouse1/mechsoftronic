/////////////////////////////////////////////////////////////////////////////
/// @file   command_processor.cpp
///
/// @date	27 avr. 2015 20:33:38
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "command_processor.hpp"

CommandProcessor::CommandProcessor()
{
	this->selected_command = JOG_Z;
}


int CommandProcessor::input_command(alt_u8 command, string payload)
{
	cout<<"command "<<command<<" payload "<<payload<<endl;
	switch (command)
	{
	case(JOG_Z):
			cout<<"JOG_Z"<<endl;
		break;
	case(JOG_Y):
			cout<<"JOG_Y"<<endl;
		break;
	case(JOG_X):
			cout<<"JOG_X"<<endl;
		break;
	case(JOG_XY):
			cout<<"JOG_XY"<<endl;
		break;
	default:
		cout<<"unrecognized command received"<<endl;
		break;
	}
}
