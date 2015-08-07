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
//#include "machine_shared_data.hpp"

extern "C"
{
#include "stdio.h"

}
CommandProcessor::CommandProcessor()
{
}

/////////////////////////////////////////////////////////////////////////////
///@brief	converts a string of ascii characters into 32 bits
///
///@details	For example: an ascii string with character sequence
///			ascii(21),ascii(7)
///			will return a u16 00010101 00000111
///			index selects which 2 bytes is converted to 16, for example
///			setting index = 0 will return the first 2 bytes in a string
///			setting index = 1 will return the next 2 bytes in a string
/////////////////////////////////////////////////////////////////////////////
alt_u16 CommandProcessor::get_word_from_string(string in_string, alt_u8 index)
{
	alt_u8 start = index * 2;
	alt_u8 stop  = start + 2;
	alt_u32 data = 0;
	alt_u32 temp = 0;
	for(int i = start; i < stop; ++i)
	{
		temp = int((unsigned char)in_string[i]);

		data =  data << 8;
		data = data | temp;

		//cout<<"dataa "<<int(data)<<" temp "<<temp<<endl;
	}
	//cout<<"data "<<data<<endl;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
///@brief	converts a string of ascii characters into 32 bits
///
///@details	For example: an ascii string with character sequence
///			ascii(128),ascii(52),ascii(21),ascii(7)
///			will return a u32 10000000001101000001010100000111
///			index selects which 4 bytes is converted to u32, for example
///			setting index = 0 will return the first four bytes in a string
///			setting index = 1 will return the second four bytes in a string
/////////////////////////////////////////////////////////////////////////////
alt_u32 CommandProcessor::get_long_from_string(string in_string, alt_u8 index)
{
	alt_u8 start = index * 4;
	alt_u8 stop  = start + 4;
	alt_u32 data = 0;
	alt_u32 temp = 0;
	for(int i = start; i < stop; ++i)
	{
		temp = int((unsigned char)in_string[i]);

		data =  data << 8;
		data = data | temp;

		//cout<<"dataa "<<int(data)<<" temp "<<temp<<endl;
	}
	//cout<<"data "<<data<<endl;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
///@brief	converts a string of ascii characters into 32 bits
///
///@details	For example: an ascii string with character sequence
///			ascii(128),ascii(52),ascii(21),ascii(7)
///			will return a u32 10000000001101000001010100000111
///			index selects which 4 bytes is converted to u32, for example
///			setting index = 0 will return the first byte in a string
///			setting index = 1 will return the second byte in a string
/////////////////////////////////////////////////////////////////////////////
alt_u8 CommandProcessor::get_byte_from_string(string in_string, alt_u8 index)
{
	return alt_u8((unsigned char)in_string[index]);
}

/////////////////////////////////////////////////////////////////////////////
///@brief takes string payload and converts to struct of dir and step count
/////////////////////////////////////////////////////////////////////////////
cnc_stepdir CommandProcessor::get_step_and_dir(string payload)
{
	cnc_stepdir stepdir;
	alt_u32 value = this->get_long_from_string(payload,0);
	//cout<<"value = "<<value<<endl;
	stepdir.data.ULONG = value;
	return stepdir;
}

/////////////////////////////////////////////////////////////////////////////
///@brief takes u32 value and converts to struct of dir and step count
/////////////////////////////////////////////////////////////////////////////
cnc_stepdir CommandProcessor::get_step_and_dir(alt_u32 value)
{
	cnc_stepdir stepdir;
	//cout<<"value = "<<value<<endl;
	stepdir.data.ULONG = value;
	return stepdir;
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	jog the axis in the direction and number of steps specified in
///			the payload.
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::jog_z(string payload)
{
	cnc_stepdir stepdiraxis;
	stepdiraxis = this->get_step_and_dir(payload);
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionZ = stepdiraxis.data.bits.dir?1:0;
	this->StepNumZ = stepdiraxis.data.bits.step;
	this->MoveZ();
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	jog the axis in the direction and number of steps specified in
///			the payload.
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::jog_y(string payload)
{
	cnc_stepdir stepdiraxis = this->get_step_and_dir(payload);
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionY = stepdiraxis.data.bits.dir?1:0;
	this->StepNumY = stepdiraxis.data.bits.step;
	this->MoveY();
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	jog the axis in the direction and number of steps specified in
///			the payload.
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::jog_x(string payload)
{
	cnc_stepdir stepdiraxis = this->get_step_and_dir(payload);
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionX = stepdiraxis.data.bits.dir?1:0;
	this->StepNumX = stepdiraxis.data.bits.step;
	this->MoveX();
}
/////////////////////////////////////////////////////////////////////////////
///@brief 	jog the axes in the direction and number of steps specified in
///			the payload.
///@details	the first four bytes of the string contains stepdir for x axis
///			the next four bytes of the string contains stepdir for y axis
///@todo	change to type cast from valuex to stepdirx instead of using func
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::jog_xy(string payload)
{
	alt_u32 valuex = this->get_long_from_string(payload,0);
	alt_u32 valuey = this->get_long_from_string(payload,1);
	cnc_stepdir stepdirx;
	cnc_stepdir stepdiry;
	stepdirx = this->get_step_and_dir(valuex);
	stepdiry = this->get_step_and_dir(valuey);
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionX = stepdirx.data.bits.dir?1:0;
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionY = stepdiry.data.bits.dir?1:0;
	this->StepNumX = stepdirx.data.bits.step;
	this->StepNumY = stepdiry.data.bits.step;
	this->MoveXY();
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the counts for pulse high and low
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_pw_z(string payload)
{
	alt_u32 valueh = this->get_long_from_string(payload,0);
	alt_u32 valuel = this->get_long_from_string(payload,1);
	this->WritePulseInfoZ(valueh,valuel);
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the counts for pulse high and low
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_pw_y(string payload)
{
	alt_u32 valueh = this->get_long_from_string(payload,0);
	alt_u32 valuel = this->get_long_from_string(payload,1);
	this->WritePulseInfoY(valueh,valuel);
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the counts for pulse high and low
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_pw_x(string payload)
{
	alt_u32 valueh = this->get_long_from_string(payload,0);
	alt_u32 valuel = this->get_long_from_string(payload,1);
	this->WritePulseInfoX(valueh,valuel);
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the counts for pulse high and low
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_pw_feed(string payload)
{
	alt_u32 value = this->get_long_from_string(payload,0);
	printf("set feed rate to %lu\n",value);
	this->WritePulseInfoFeed(value);
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the counts for pulse high and low
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_coordinate(string payload)
{
	alt_32 value1 = (alt_32)this->get_long_from_string(payload,0);
	alt_32 value2 = (alt_32)this->get_long_from_string(payload,1);
	//printf("x = %lu, y = %lu",value1,value2);
	this->SetNextPosition(value1,value2);
}


/////////////////////////////////////////////////////////////////////////////
///@brief 	set the acceleration data
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_acceleration(string payload)
{
	alt_u32 value1 = this->get_long_from_string(payload,0);
	alt_u32 value2 = this->get_long_from_string(payload,1);
	this->SetAcceleration(value1,value2);
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the layer data
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_layer(string payload)
{
    printf("set layer!\n");
	this->LayerNumber  = this->get_word_from_string(payload,0);
	this->LayerThickness = this->get_word_from_string(payload,1);
}

/////////////////////////////////////////////////////////////////////////////
///@brief   set the layer data
/////////////////////////////////////////////////////////////////////////////
void CommandProcessor::set_router_state(string payload)
{
    this->AppendStateToRoutes((Peripheral)this->get_byte_from_string(payload,0));
}


/////////////////////////////////////////////////////////////////////////////
///@brief 	process command, given command number and payload string
/////////////////////////////////////////////////////////////////////////////
int CommandProcessor::input_command(alt_u8 command, string payload)
{

	//cout<<"command "<<command<<" payload "<<payload<<" payload-int "<<int((unsigned char)payload[0])<<endl;
	switch (command)
	{
	case(JOG_Z):
		this->jog_z(payload);
		break;
	case(JOG_Y):
		this->jog_y(payload);
		break;
	case(JOG_X):
		this->jog_x(payload);
		break;
	case(JOG_XY):
		this->jog_xy(payload);
		break;
	case(SET_PW_Z):
		this->set_pw_z(payload);
		break;
	case(SET_PW_Y):
		this->set_pw_y(payload);
		break;
	case(SET_PW_X):
		this->set_pw_x(payload);
		break;
	case(G_XY):
		//printf("GCode XY\n");
		this->set_coordinate(payload);
		break;
	case(START_ROUTE):
		this->StartRouting();
		break;
	case(FEED):

		this->set_pw_feed(payload);
		break;
	case(ERASE_COORD):
        printf("coordinates erased\n");
		this->routes.clear();
		cout<<"route cleared!"<<endl;
		break;

	case(SET_LAYER):
		this->set_layer(payload);
		break;
    case(SET_ACCEL):
        this->set_acceleration(payload);
        break;
    case(SET_ROUTE_STATE):
        this->set_router_state(payload);
        break;
    case(G_Z):
		printf("G code Z\n");
    	this->SetNextZPosition((alt_32)this->get_long_from_string(payload,0));
        break;
	default:
		printf("unrecognized command received %d\n",command);

		return 1;
	}
	return 0;
}
