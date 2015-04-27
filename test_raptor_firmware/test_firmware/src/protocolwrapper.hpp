/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Sep 8, 2014 3:18:48 PM
///
/// @author	Michael House
///
/// @brief	[Brief one sentence description.]
///
/// [Detailed description of file, optional.]
///
/// @ingroup [SUBSYSTEM_PREFIX]
///
/// @par Copyright (c) 2014 All rights reserved.

#ifndef PROTOCOLWRAPPER_HPP_
#define PROTOCOLWRAPPER_HPP_
#include <iostream>
#include <string>
#include <list>

using namespace std;
typedef unsigned char UBYTE;
enum protocol_decode {WAIT_HEADER, IN_MSG, AFTER_DLE};
enum protocol_status {START_MSG, BUILDING_MSG, MSG_OK, ERROR};

class ProtocolWrapper
{
public:
    ProtocolWrapper();
    string wrap(list<string> fields);
    protocol_decode decode_state;
    protocol_status pstatus;
    int input(UBYTE  byte);
    string message_buf;
    string last_message;
    list<string> get_fields(void);

private:
    UBYTE Header;
    UBYTE Dle;
    UBYTE Footer;

    int crc_value;

    bool keep_header;
    bool keep_footer;

    int finish_msg(void);
};



#endif /* PROTOCOLWRAPPER_HPP_ */
