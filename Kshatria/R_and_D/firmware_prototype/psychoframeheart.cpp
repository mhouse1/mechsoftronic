/////////////////////////////////////////////////////////////////////////////
/// @file   psychoframeheart.cpp
///
/// @date	21 sept. 2014 06:59:01
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
///
/// @history 04/18/2015 added support for setting z axis dir and step number
/////////////////////////////////////////////////////////////////////////////
#include <list>
#include "psychoframeheart.hpp"
#include <string>
extern "C"
{
#include "stdio.h"
#include "stdlib.h"
#include "crc.h"
#include "includes.h"


}


using namespace std;
/////////////////////////////////////////////////////////////////////////////
///@brief uses PsychoFrame to get a list of fields lists
///		  the goal is to isolate PsychoFrame from the processing mechanism
///		  allowing users of PsychoFrame to have multiple processing mechanisms
/////////////////////////////////////////////////////////////////////////////
alt_u8 PsychoFrameHeart::input(string data)
{
	PsychoFrame pf;
	list<list<string> > decoded;
	list<string> layer1_fields;
	list<string> layer2_fields;
	decoded = pf.data_in(data);
	layer1_fields = decoded.front();
	process_layer1(layer1_fields);
	decoded.pop_front();
	//print_fields(layer1_fields);
	layer2_fields = decoded.front();
	process_layer2(layer2_fields);
	decoded.pop_front();
	//print_fields(layer2_fields);

	//verify all items processed, return 1 if false
	if(!decoded.empty()) { return 1; }

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
///@brief verify received crc against message
/////////////////////////////////////////////////////////////////////////////
alt_u8 PsychoFrameHeart::process_layer1(list<string> fields_list)
{
	string crc;
	string layer2_encoded_message;

	//pop the crc from front of fields list
	crc = fields_list.front();
	fields_list.pop_front();

	//pop the message from fron of fields list
	layer2_encoded_message = fields_list.front();
	fields_list.pop_front();

	//verify all items processed/popped front fields_list
	///return 1 if false
	if(!fields_list.empty()) { return 1; }

	//calculate crc_value for layer2_encoded_message
	alt_u32 last_crc_value;
	last_crc_value = this->crc_value;
	this->crc_value = crc32((char *) layer2_encoded_message.c_str(),layer2_encoded_message.length(),last_crc_value);

	//Converts calculated crc_value into hex string crc_string
	ULONG ulVal, ulShift, ulDigit;
	signed short m;
	ulVal = this->crc_value;
	ulShift = 28;
	string crc_string = "";
	for (m = 8; m > 0; m--)
	{
	    ulDigit = (ulVal >> ulShift) & 0x0000000F;	// Get nibble of interest
	    if (ulDigit <= 9)
		  //cout<<(char)(ulDigit + '0');
	    	crc_string += (char)(ulDigit + '0');
	    else
		  //cout<<(char)(ulDigit - 10 + 'A');
	    	crc_string +=(char)(ulDigit - 10 + 'A');
	    ulShift -=4;
	}
	//printf("this->crc_value =%s \n",crc_string.c_str());

	//check that received crc matches calculated crc
	//calculated_crc = crc(layer2_encoded_message)
	if (!(crc == crc_string))
	{
		printf("crc mismatch!\n");
		return 1;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	process data based on value defined in the classification field
///
///@details data will contain a classification string, each class of data will
///		    have its own processing function to call.
/////////////////////////////////////////////////////////////////////////////
alt_u8 PsychoFrameHeart::process_layer2(list<string> fields_list)
{
	string classification;
	classification = fields_list.front();
	fields_list.pop_front();
	//type = fields_list.front();

	//@todo replace classification strings with enums
	//this will optimize by decreasing number of
	//characters sent over serial
	if (classification == "cfdata")
	{
		process_cfdata(fields_list);
	}
	else if ( classification =="gcode")
	{
		process_gcode(fields_list);
	}
	else if ( classification =="command")
	{
		process_command(fields_list);
	}
	else
	{
		//return 2;
	}
	return 0;
}

//[#[C98074B8#]#[###[cfdata###]###[HMin###]###[2000###]#]]
//[#[958D3E83#]#[###[cfdata###]###[HMax###]###[3000###]#]]
//[#[75755559#]#[###[cfdata###]###[LMin###]###[2100###]#]]
//[#[D8834C4F#]#[###[cfdata###]###[LMax###]###[3100###]#]]
//[#[8A828489#]#[###[cfdata###]###[SPer###]###[50###]#]]
//[#[CE064E32#]#[###[cfdata###]###[HVal###]###[2500###]#]]
//[#[EE52EE94#]#[###[cfdata###]###[LVal###]###[2600###]#]]
//[#[77E292F1#]#[###[cfdata###]###[SInc###]###[10###]#]]
/////////////////////////////////////////////////////////////////////////////
///@brief 	process the configuration data
///
///@details typically sets constant values
/////////////////////////////////////////////////////////////////////////////
alt_u8 PsychoFrameHeart::process_cfdata(list<string> fields_list)
{
	string type;
	type = fields_list.front();
	fields_list.pop_front();
	string data;
	data = fields_list.front();
	fields_list.pop_front();
	if (type == "HMin")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		//convert
		this->SetHighPulseWidthMin((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "HMax")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetHighPulseWidthMax((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "LMin")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetLowPulseWidthMin((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "LMax")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetLowPulseWidthMax((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "SPer")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetSpeedVal((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "HVal")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetHighPulseWidthVal((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "LVal")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetLowPulseWidthVal((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "SInc")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetMaxSpeed((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "StepNumX")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetNumberOfStepsX((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "StepNumY")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetNumberOfStepsY((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "StepNumZ")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		this->SetNumberOfStepsZ((alt_u32)atoi((char *)data.c_str()));
	}
	else if (type == "DirX")
	{
		printf("%s data = %s\n",type.c_str(), data.c_str());

		Direction dirx;
		dirx = (data == "up" )? up : down;
		this->MotorXDir(dirx);//((alt_u16)atoi((char *)data.c_str()));
	}
	else if (type == "DirY")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		Direction diry;
		diry = (data == "up" )? up : down;
		this->MotorYDir(diry);//((alt_u16)atoi((char *)data.c_str()));
	}
	else if (type == "DirZ")
	{
		//printf("%s data = %s\n",type.c_str(), data.c_str());
		Direction dirz;
		dirz = (data == "up" )? up : down;
		this->MotorZDir(dirz);//((alt_u16)atoi((char *)data.c_str()));
	}
	else if (type == "RouterPWM")
	{
//		int i = 1640;
//		for (i = 1640; i <3000; i++)
//		{
//			OSTimeDlyHMSM(0, 0, 0, 100);
//			this->WriteRouterPWM((alt_u32) i);
//		}
//		OSTimeDlyHMSM(0, 0, 1, 0);
		//printf ("done \n");
		printf("%s data router pwm = %s\n",type.c_str(), data.c_str());
		this->WriteRouterPWM((alt_u32)atoi((char *)data.c_str()));
	}
	else
	{
		printf("invaid cfdata with type = %s \n",type.c_str());
		//return 2;
	}
	return 0;
}

//[#[8885F03D#]#[###[gcode###]###[G90###]#]]
//[#[D3BFEF5A#]#[###[gcode###]###[G21###]#]]
//[#[6BCC3A67#]#[###[gcode###]###[G0###]###[X6.4844###]###[Y8.4801###]#]]
//[#[79EEEE0F#]#[###[gcode###]###[M03###]#]]
//[#[5867A4B0#]#[###[gcode###]###[G1###]###[F30.000000###]#]]
//[#[3073076F#]#[###[gcode###]###[G1###]###[X29.4366###]###[Y8.4801###]#]]
//[#[857B4A14#]#[###[gcode###]###[G1###]###[X17.552###]###[Y30.7195###]#]]
//[#[329FEDE8#]#[###[gcode###]###[G1###]###[X6.484###]###[Y8.48###]#]]
//[#[97518E46#]#[###[gcode###]###[M05###]#]]
//[#[96A149AD#]#[###[gcode###]###[G0###]###[X0.000###]###[Y0.000###]#]]
//[#[B147E607#]#[###[gcode###]###[M05###]#]]
//[#[D22EF06B#]#[###[gcode###]###[M02###]#]]
alt_u8 PsychoFrameHeart::process_gcode(list<string> fields_list)
{
	string type;
	type = fields_list.front();
	fields_list.pop_front();
//	string data;
//	data = fields_list.front();
//	fields_list.pop_front();
	if ((type == "G0") ||(type == "G1") )
	{

		//printf("type =  %s \n",type.c_str());
		string data;
		data = fields_list.front();
		fields_list.pop_front();
		if ('F' == data[0])
		{
			return 0;
		}
		alt_u32 xpos, ypos;
		//printf("XData =  %s \n",data.c_str()); //X6.4844
		//printf("XData =  %s \n",data.c_str()+1); //6.4844
		//printf("XData =  %lu \n",(alt_u32)atoi((char *)data.c_str()+1)); //6
		xpos = (alt_u32)(atof((char *)data.c_str()+1)*10000);
		printf("XData =  %lu \n",xpos); //64843
		data = fields_list.front();
		fields_list.pop_front();
		//printf("YData =  %s \n",data.c_str());
		ypos = (alt_u32)(atof((char *)data.c_str()+1)*10000);
		printf("YData =  %lu \n",ypos);
		//this->SetNumberOfStepsX((alt_u32)atoi((char *)data.c_str()));
		this->SetNextPosition(xpos,ypos);
	}
	else if (type == "G90")
	{
		//clear
		this->ClearRoute();
		printf("routes cleared\n");
	}
	else if (type == "M02")
	{
		printf("nothing here yet for GCode type M02\n");
		//printf("started routing\n");
		//this->StartRouting();
	}
	else if (type == "M03")
	{
		this->router_state = on;
		printf("router on\n");
	}
	else if (type == "M05")
	{
		this->router_state = off;
		printf("router off\n");
	}
	else
	{
		printf("undefined type =  %s \n",type.c_str());
		//return 2;
	}
	return 0;
}

alt_u8 PsychoFrameHeart::process_command(list<string> fields_list)
{
	string type;
	type = fields_list.front();
	fields_list.pop_front();
//	string data;
//	data = fields_list.front();
//	fields_list.pop_front();
	if (type == "MoveXY")//@todo change to use string MoveXY
	{
		//moves in xy direction simultaneously
		printf("type =  %s \n",type.c_str());
		this->Move();
	}
	else if (type == "MoveZ")
	{
		//moves in xy direction simultaneously
		printf("type =  %s \n",type.c_str());
		this->MoveZ();
	}
	else if (type == "Clear")
	{
		printf("type =  %s \n",type.c_str());
	}
	else if (type == "Start_Routing")
	{
		printf("type =  %s \n",type.c_str());
		this->StartRouting();
	}
	else if (type =="write_settings")
	{
		this->WriteSettings();
	}
	else
	{
		printf("undefined type =  %s \n",type.c_str());
		//return 2;
	}
	return 0;
}

void PsychoFrameHeart::print_fields(list<string> fields_list)
{
	printf("\n");
	list<string>::iterator it;
	string fld;
	int counter = 0;
	//print fields
	for (it = fields_list.begin(); it != fields_list.end(); it++)
	{
		counter++;
		fld = (string) *it;
		printf("field%d = %s\n",counter, fld.c_str());
	}
}
