/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Sep 19, 2014 12:57:35 PM
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

//#include <iostream>

#include "types.hpp"
#include "cncmachine.hpp"
extern "C"
{
#include "system.h"
#include "stdio.h"
#include "altera_avalon_pio_regs.h"
#include "slave_template_macros.h"

}
#include <cmath>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
///@brief constructor function that sets initial value of object data
/////////////////////////////////////////////////////////////////////////////
CncMachine::CncMachine()
{
	this->StepPosX			= 0;
	this->StepPosY			= 0;
	this->PresentX          = 0;
	this->PresentY          = 0;
	this->NextX             = 0;
	this->NextY             = 0;
	this->StepNumX          = 50;
	this->StepNumY          = 50;
	this->MaxSpeed          = 0;
	this->SpeedVal          = 0;
	this->HighPulseWidthMin = 0;
	this->HighPulseWidthVal = 0;
	this->HighPulseWidthMax = 0;
	this->LowPulseWidthMin  = 0;
	this->LowPulseWidthVal  = 0;
	this->LowPulseWidthMax  = 0;

	this->FullRangeStepCount= 2200;
	this->FullRangeDistance = 380000; //38mm scaled by 10000
	CNC_CONTROL.CTRL.ULONG  = 0;
	//this->ControlRegister	= 0;
}

/////////////////////////////////////////////////////////////////////////////
///@brief This Section is dedicated to Set functions
/////////////////////////////////////////////////////////////////////////////
void CncMachine::SetCurrentPosition(alt_u32 x, alt_u32 y)
{
	this->PresentX = x;
	this->PresentY = y;
}

/////////////////////////////////////////////////////////////////////////////
///@brief given the next coordinate, calculate number of steps and direction
///@details returns an non zero exit if error
/// Exit code 0: NORMAL_EXIT
///		there are no errors
/// Exit code 1: INVALID_COORDINATE
///		input coordinate is not in range, should be <= to FullRangeDistance
/// Exit code 2: INVALID_STEPCOUNT
///		for a given direction there is a certain amount of steps leftover to
///		to move, if the Step number calculated is not in range of steps
///		leftover for a given direction then return with error code
/////////////////////////////////////////////////////////////////////////////
alt_u8 CncMachine::SetNextPosition(alt_u32 x, alt_u32 y)
{
	//@todo NextX, NextY variables might not be necessary
	//since we're going to set this->PresentX = this->NextX
	if (!(x <= this->FullRangeDistance) || !(y <= this->FullRangeDistance))
	{
		//cout<<"coordinate not in range"<<endl;
		return 1;
	}


	this->NextX = x;
	this->NextY = y;

	//Convert distance to steps
	//calculate distance
	alt_32 distanceX = this->NextX - this->PresentX;
	alt_32 distanceY = this->NextY - this->PresentY;
	//steps_needed = full_range_step_count*(distanceX/full_range_distance)
//	int absx = fabs(distanceX);
//	int absy = fabs(distanceY);
	this->StepNumX = (this->FullRangeStepCount*(alt_32)fabs(distanceX))/this->FullRangeDistance;
	this->StepNumY = (this->FullRangeStepCount*(alt_32)fabs(distanceY))/this->FullRangeDistance;
	//calculate direction
//	Direction xAxis, yAxis;
	MotorXDir(distanceX >= 0? up:down);
	MotorYDir(distanceY >= 0? up:down);
//	this->StepPosX = this->StepPosX +
//			(this->StepNumX *this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionX == up?1:-1);
	this->PresentX = this->NextX;
	this->PresentY = this->NextY;

	return 0;
}


void CncMachine::MotorXDir(Direction x)
{
	//set X axis direction
	printf("dirX set =  %d\n",x);
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionX = x;
}
void CncMachine::MotorYDir(Direction y)
{
	//set Y axis direction
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionY = y;
}
void CncMachine::SetMaxSpeed(alt_u32 speed)
{
	this->MaxSpeed = speed;
}
void CncMachine::SetSpeedVal(alt_u32 speed)
{
	this->SpeedVal = speed;
}
void CncMachine::SetHighPulseWidthMin(alt_u32 width)
{
	this->HighPulseWidthMin = width;
	printf("set High Pulse Width Min = %lu\n",width);
}
void CncMachine::SetHighPulseWidthVal(alt_u32 width)
{
	this->HighPulseWidthVal = width;
	printf("set High Pulse Width Val = %lu\n",this->HighPulseWidthVal);
}
void CncMachine::SetHighPulseWidthMax(alt_u32 width)
{
	this->HighPulseWidthMax = width;
	printf("set High Pulse Width Max = %lu\n",width);
}
void CncMachine::SetLowPulseWidthMin(alt_u32 width)
{
	this->LowPulseWidthMin = width;
}
void CncMachine::SetLowPulseWidthVal(alt_u32 width)
{
	this->LowPulseWidthVal = width;
	printf("set High Pulse Width Val = %lu\n",this->HighPulseWidthVal);
}
void CncMachine::SetLowPulseWidthMax(alt_u32 width)
{
	this->LowPulseWidthMax = width;
}
void CncMachine::SetNumberOfStepsX(alt_u32 steps)
{
	this->StepNumX = steps;
}
void CncMachine::SetNumberOfStepsY(alt_u32 steps)
{
	this->StepNumY = steps;
}

/////////////////////////////////////////////////////////////////////////////
///@brief function is mainly used during development to see api data
/////////////////////////////////////////////////////////////////////////////
CncMachine::TRAVERSALXY CncMachine::GetXYMovement()
{
	TRAVERSALXY data;
	data.X.Position = this->PresentX;
	data.X.StepDir  = this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionX;
	data.X.StepNum	= this->StepNumX;
	data.Y.Position = this->PresentY;
	data.Y.StepDir  = this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionY;
	data.Y.StepNum	= this->StepNumY;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
///@brief Write setting value for each stepper motor into register
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WriteSettings()
{
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_0*4),this->HighPulseWidthVal);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_1*4),this->LowPulseWidthVal );
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_2*4),this->StepNumX          );
   //IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_3*4),0); //Control register
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_4*4),this->HighPulseWidthVal);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_5*4),this->LowPulseWidthVal );
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_6*4),this->StepNumY           );
}

void CncMachine::Move()
{
	//write pulse settings to memory mapped RAM
	WriteSettings();

	//Toggle run bit
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 1;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 0;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 0;
	WriteControlRegister();
}

/////////////////////////////////////////////////////////////////////////////
///@brief clears the bits in CNC control register
/////////////////////////////////////////////////////////////////////////////
void CncMachine::ClearControlRegister()
{
	//this->ControlRegister = 0;
	//CNC_CTRL->
	this->CNC_CONTROL.CTRL.ULONG = 0;
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_3*4),this->CNC_CONTROL.CTRL.ULONG);
}

void CncMachine::WriteControlRegister()
{
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_3*4),this->CNC_CONTROL.CTRL.ULONG);
}


