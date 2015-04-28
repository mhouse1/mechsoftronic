/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Apr 27, 2015 12:46:32 PM
///
/// @author	Michael House
///
/// @brief	a simple decoder protocol similar to ones used on TCP socket
///
/// [Detailed description of file, optional.]
///
/// @ingroup [SUBSYSTEM_PREFIX]
///
/// @par Copyright (c) 2015 All rights reserved.

#ifndef COMMUNICATION_TCP_BASED_HPP_
#define COMMUNICATION_TCP_BASED_HPP_

#include <string>

#include "types.hpp"
#include "command_processor.hpp"

using namespace std;

typedef unsigned char UBYTE;

enum state_decode {WAIT_COMMAND,WAIT_LENGTH,WAIT_MSG};
enum state_status {STATUS_START,RECEIVED_COMMAND,RECEIVED_LENGTH,BUILDING_MSG,DONE,ERROR};

class CommSimple : public CommandProcessor
{
public:

	state_decode decode_state;
	state_status decode_status;


	CommSimple();
	int input(UBYTE byte);

private:
	alt_u8 msg_command;
	alt_u8 msg_length;
	alt_u16 msg_received_index;
	string msg_payload;

};


#endif /* COMMUNICATION_TCP_BASED_HPP_ */
