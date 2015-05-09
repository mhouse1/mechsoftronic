/////////////////////////////////////////////////////////////////////////////
/// @file   test_cncmachine.cpp
///
/// @date	26 avr. 2015 20:23:58
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include "cute.h"
#include "test_cncmachine.h"

#include <list>
#include "types.hpp"
#include "cncmachine.hpp"
#include <iostream>

using namespace std;


typedef struct
{
	alt_u32 x;
	alt_u32 y;
} COORDINATE;


COORDINATE xy(alt_u32 x, alt_u32 y)
{
	COORDINATE pos;
	pos.x = x;
	pos.y = y;
	return pos;
}

/////////////////////////////////////////////////////////////////////////////
///@brief verify cncmachine sets the correct direction
///@details creates a list of coordinates then send it into cncmachine object
///			compare old coordinate with new coordinate to verify direction
/////////////////////////////////////////////////////////////////////////////
void InputCoordinateTest() {
	list<COORDINATE> triangle;
	triangle.push_back(xy(294366,84801));
	triangle.push_back(xy(175520,307195));
	triangle.push_back(xy(64840,380000));
	triangle.push_back(xy(0,0));

	list<COORDINATE>::iterator it;
	CncMachine raptor;
	COORDINATE pos;
	//COORDINATE old;
	CncMachine::TRAVERSALXY data;
	//CncMachine::Direction dir;
//	alt_32 val;
	//old.x = 0;
	//old.y = 0;
	for(it = triangle.begin(); it != triangle.end(); it++)
	{
		pos = *it;
		raptor.SetNextPosition(pos.x,pos.y);
		data = raptor.GetXYMovement();
		//compare with previous position to new direction is
		//set correctly with respect to old position
		//verify X direction
//		val = (alt_32)data.X.Position - (alt_32)old.x;
//		cout << "value position change = "<<val<<endl;
		//dir = ((alt_32)data.X.Position - (alt_32)old.x)>0?
		//		CncMachine::up: CncMachine::down;
		//ASSERT_EQUAL(dir,data.X.StepDir);
		//verify Y direction
		//dir = ((alt_32)data.Y.Position - (alt_32)old.y)>0?
		//		CncMachine::up: CncMachine::down;
		//ASSERT_EQUAL(dir,data.Y.StepDir);

		//old.x = data.X.Position;
		//old.y = data.Y.Position;

	}
}

void outOfRangeTest()
{
	//ASSERTM("start writing tests", false);
	CncMachine machine;
	//send triangle coordinates , triangle.nc
	machine.SetNextPosition(380001,84801);
	ASSERT_EQUAL(1,(int)machine.SetNextPosition(380001,84801));


}



void routeTest()
{
	CncMachine machine;
	//send triangle coordinates , triangle.nc
	machine.SetNextPosition(294366,84801);
	machine.SetNextPosition(175520,307195);
	machine.SetNextPosition(64840,84800);

	machine.StartRouting();
}


cute::suite make_suite_test_cncmachine(){
	cute::suite s;

	//push_back tests to s
//	s.push_back(CUTE(outOfRangeTest));
//	s.push_back(CUTE(InputCoordinateTest));
//	s.push_back(CUTE(thisIsATest));
	s.push_back(CUTE(routeTest));
	return s;
}



