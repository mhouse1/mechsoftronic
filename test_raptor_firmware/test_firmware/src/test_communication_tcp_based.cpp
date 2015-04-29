/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Apr 27, 2015 1:02:38 PM
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "cute.h"
#include "test_communication_tcp_based.h"
#include "communication_tcp_based.hpp"


using namespace std;

void testReceiver()
{
	CommSimple listener;

	//JOGZ DIR =1 STEP = 000 0000 0000 0000
	listener.input(0);     //command
	listener.input(4);     //length
	listener.input(128);
	listener.input(52);
	listener.input(21);
	listener.input(7);

	//JOGY DIR =1 STEP = 000 0000 0000 0000
	listener.input(1);      //command
	listener.input(4);      //length
	listener.input(128);
	listener.input(0);
	listener.input(0);
	listener.input(0);

	//JOGX DIR =1 STEP = 000 0000 0000 0000
	listener.input(2);      //command
	listener.input(4);      //length
	listener.input(128);
	listener.input(0);
	listener.input(0);
	listener.input(0);

	//JOGXY DIR =1 STEP = 000 0000 0000 0000
	listener.input(3); 		//command
	listener.input(8); 		//length
	listener.input(128);
	listener.input(0);
	listener.input(0);
	listener.input(0);
	listener.input(0);
	listener.input(0);
	listener.input(0);
	listener.input(0);

	listener.input(3);     //command
	listener.input(8);     //length
	listener.input('h');
	listener.input('i');
	listener.input('j');
	listener.input(0);
	listener.input(0);
	listener.input(3);
	listener.input(2);
	listener.input(1);
}

cute::suite make_suite_test_comm_tcp_based(){
	cute::suite s;

	//push_back tests to s
	s.push_back(CUTE(testReceiver));
	return s;
}

