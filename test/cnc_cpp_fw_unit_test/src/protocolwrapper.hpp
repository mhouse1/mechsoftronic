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
typedef unsigned long ULONG;

enum protocol_decode {WAIT_HEADER, IN_MSG, AFTER_DLE};
enum protocol_status {START_MSG, BUILDING_MSG, MSG_OK, ERROR};

class ProtocolWrapper
{
public:

    protocol_decode decode_state;
    protocol_status pstatus;
    string message_buf;
    string last_message;

    ProtocolWrapper();
    string wrap(list<string> fields);
    int input(UBYTE  byte);
    list<string> get_fields(void);
    list<string> get_fields(string message);
    string get_lastmessage();

private:
    UBYTE Header;
    UBYTE Dle;
    UBYTE Footer;

    ULONG crc_value;

    bool keep_header;
    bool keep_footer;

    int finish_msg(void);
};



#endif /* PROTOCOLWRAPPER_HPP_ */
