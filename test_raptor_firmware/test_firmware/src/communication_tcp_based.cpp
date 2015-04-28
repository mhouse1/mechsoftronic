/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Apr 27, 2015 12:45:54 PM
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2015 All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "communication_tcp_based.hpp"


using namespace std;

CommSimple::CommSimple()
{
	this->decode_state = WAIT_COMMAND;
	this->decode_status = STATUS_START;
	this->msg_command = 0;
	this->msg_length = 0;
	this->msg_received_index = 0;
}


int CommSimple::input(UBYTE new_byte)
{
	switch(this->decode_state)
	{
	case(WAIT_COMMAND):
		cout<<"command received char: "<<new_byte<<" received int: "<<static_cast<int>(new_byte)<<endl;
		this->msg_command = static_cast<int>(new_byte);
		this->decode_state = WAIT_LENGTH;
		this->msg_received_index = 0;
		return this->decode_status = RECEIVED_COMMAND;
	case(WAIT_LENGTH):
		cout<<"length received char: "<<new_byte<<" received int: "<<static_cast<int>(new_byte)<<endl;
		this->msg_length = static_cast<int>(new_byte);
		this->decode_state = WAIT_MSG;
		return this->decode_status = RECEIVED_LENGTH;
	case(WAIT_MSG):
		this->msg_received_index += 1;
		cout<<"msg received "<<new_byte<<" index "<<this->msg_received_index<<endl;

		this->msg_payload += new_byte;
		if (this->msg_received_index < this->msg_length)
		{


			return this->decode_status = BUILDING_MSG;
		}
		else //full message received, process message then go to next state
		{
			//process_message(command,payload);
			this->input_command(this->msg_command,this->msg_payload);
			this->decode_state = WAIT_COMMAND;
			return this->decode_status = DONE;
		}

	default:
		return this->decode_status = ERROR;
	}

}
