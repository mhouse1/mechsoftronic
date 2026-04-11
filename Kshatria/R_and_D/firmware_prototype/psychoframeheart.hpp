/////////////////////////////////////////////////////////////////////////////
/// @file   psychoframeheart.hpp
///
/// @date	21 sept. 2014 06:59:48
///
/// @author	Michael House
///
/// @brief
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef PSYCHOFRAMEHEART_HPP_
#define PSYCHOFRAMEHEART_HPP_
#include "types.hpp"
#include "protocolwrapper.hpp"
#include "cncmachine.hpp"
#include "psychoframe.hpp"

#include <string>
extern "C"
{
#include "stdio.h"
}

class PsychoFrameHeart: public CncMachine
{
public:
	PsychoFrameHeart()
	{
		this->crc_value = 740712820;
	}
	alt_u8 input(string data);
	void print_fields(list<string> fields_list);
	alt_u32 crc_value;
private:
	alt_u8 process_layer1(list<string> fields_list);
	alt_u8 process_layer2(list<string> fields_list);
	alt_u8 process_cfdata(list<string> fields_list);
	alt_u8 process_gcode(list<string> fields_list);
	alt_u8 process_command(list<string> fields_list);

};



#endif /* PSYCHOFRAMEHEART_HPP_ */
