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
///
/// @history 04/18/2015 added function to set z axis direction and steps
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
	this->StepPosZ			= 0;
	this->PresentX          = 0;
	this->PresentY          = 0;
	this->PresentZ          = 0;
	this->NextX             = 0;
	this->NextY             = 0;
	this->NextZ             = 0;
	this->StepNumX          = 50;
	this->StepNumY          = 50;
	this->StepNumZ          = 50;
	this->MaxSpeed          = 0;
	this->SpeedVal          = 0;
	this->HighPulseWidthMin = 0;
	this->HighPulseWidthVal = 20000;
	this->HighPulseWidthMax = 0;
	this->LowPulseWidthMin  = 0;
	this->LowPulseWidthVal  = 1500;
	this->LowPulseWidthMax  = 0;
	this->router_state = off;
	this->FullRangeStepCount= 2000;
	this->FullRangeDistance = 380000; //38mm scaled by 10000
	this->CNC_CONTROL.CTRL.ULONG  = 0;
	this->CNC_STATUS.STUS.ULONG = 0;


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
	TRAVERSALXY data;
	//@todo NextX, NextY variables might not be necessary
	//since we're going to set this->PresentX = this->NextX
	if (!(x <= this->FullRangeDistance) || !(y <= this->FullRangeDistance))
	{
		//cout<<"coordinate not in range"<<endl;
		return 1;
	}


//	this->NextX = x;
//	this->NextY = y;
	data.X.Position = x;
	data.Y.Position = y;

	//Convert distance to steps
	//calculate distance then set the new position as present position
	alt_32 distanceX = x - this->PresentX;
	alt_32 distanceY = y - this->PresentY;
	this->PresentX = x;
	this->PresentY = y;

	//steps_needed = full_range_step_count*(distanceX/full_range_distance)
//	int absx = fabs(distanceX);
//	int absy = fabs(distanceY);
	data.X.StepNum = (this->FullRangeStepCount*(alt_32)fabs(distanceX))/this->FullRangeDistance;
	data.Y.StepNum = (this->FullRangeStepCount*(alt_32)fabs(distanceY))/this->FullRangeDistance;

	//calculate total time from point A to point B
	//adjust time so fastest axis is slowed down to match slowest axis
	//code to be added here
	//pulse period
	//calculation is based on half period to prevent overflow
	alt_u32 HalfPulsePeriod = (this->HighPulseWidthVal + this->LowPulseWidthVal)/2;
	alt_u32 HalfTimeX = data.X.StepNum * HalfPulsePeriod;
	alt_u32 HalfTimeY = data.Y.StepNum * HalfPulsePeriod;
	//slow down the axis with less steps
	if (HalfTimeX > HalfTimeY) //X slower than Y
	{
		alt_u32 HalfPeriod = HalfTimeX/data.Y.StepNum; //calculate half period for Y based on X
		data.Y.HighPulseWidth = HalfPeriod;
		data.Y.LowPulseWidth = HalfPeriod;
		data.X.HighPulseWidth = this->HighPulseWidthVal;
		data.X.LowPulseWidth = this->LowPulseWidthVal;
	}
	else //Y slower than X
	{
		alt_u32 HalfPeriod = HalfTimeY/data.X.StepNum; //calculate half period for Y based on X
		data.X.HighPulseWidth = HalfPeriod;
		data.X.LowPulseWidth = HalfPeriod;
		data.Y.HighPulseWidth = this->HighPulseWidthVal;
		data.Y.LowPulseWidth = this->LowPulseWidthVal;
	}//@todo need to add checks in here to make sure that Pulses widths are in valid range



	//calculate and set direction
	//if change in distance is positive move up, else move down
//	MotorXDir(distanceX >= 0? up:down);//@todo removable if something else sets control bits
//	MotorYDir(distanceY >= 0? up:down);
	data.X.StepDir = distanceX >= 0? up:down;
	data.Y.StepDir = distanceY >= 0? up:down;
	data.router_state = this->router_state;


	this->routes.push_back(data);
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
void CncMachine::MotorZDir(Direction z)
{
	//set Y axis direction
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionZ = z;
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
	printf("set Low Pulse Width Val = %lu\n", this->LowPulseWidthVal);
}
void CncMachine::SetLowPulseWidthMax(alt_u32 width)
{
	this->LowPulseWidthMax = width;
}
void CncMachine::SetNumberOfStepsX(alt_u32 steps)
{
	this->StepNumX = steps;
	printf("RAM StepNumx = %lu\n", this->StepNumX);
}
void CncMachine::SetNumberOfStepsY(alt_u32 steps)
{
	this->StepNumY = steps;
}

void CncMachine::SetNumberOfStepsZ(alt_u32 steps)
{
	this->StepNumZ = steps;
}
list<CncMachine::TRAVERSALXY> CncMachine::GetRoutes()
{
	return this->routes;
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
	data.router_state = this->router_state;
	return data;
}

void CncMachine::WriteStepNum(alt_u32 XSteps, alt_u32 YSteps)
{
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_2*4),XSteps );
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_6*4),YSteps );
}

void CncMachine::WriteStepNumZ(alt_u32 ZSteps)
{
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_10*4),ZSteps );
}

void CncMachine::WriteRouterPWM(alt_u32 PWMVal)
{
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_7*4),PWMVal );
}

/////////////////////////////////////////////////////////////////////////////
///@brief Write XY setting value for each stepper motor into register
/// this function is deprecated, try not to use (i will be removed soon)
/// 04/14/2015 this function is called when routing begins, which may be
/// 		   unnecessary now that WritePulseInfo() is available
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WriteSettings()
{
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_0*4),this->HighPulseWidthVal); //pulse_width_high_A
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_1*4),this->LowPulseWidthVal ); //pulse_width_low_A

   //IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_3*4),0); //Control register
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_4*4),this->HighPulseWidthVal); //pulse_width_high_B
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_5*4),this->LowPulseWidthVal ); //pulse_width_low_B

	 //IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_8*4),this->HighPulseWidthVal); //pulse_width_high_C
	 //IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_9*4),this->LowPulseWidthVal ); //pulse_width_low_C

	 //may need to uncomment this if gcode file is used
	 WriteStepNum(this->StepNumX, this->StepNumY);

}

/////////////////////////////////////////////////////////////////////////////
///@brief Write pulse info for x and y axis
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfo(alt_u32 XHighPulseWidth, alt_u32 XLowPulseWidth, alt_u32 YHighPulseWidth, alt_u32 YLowPulseWidth)
{
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_0*4),XHighPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_1*4),XLowPulseWidth);

   //IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_3*4),0); //Control register
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_4*4),YHighPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_5*4),YLowPulseWidth );
	 //WriteStepNum(this->StepNumX, this->StepNumY);

}

/////////////////////////////////////////////////////////////////////////////
///@brief Write pulse info for x and y axis
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfoZ(alt_u32 ZHighPulseWidth,alt_u32 ZLowPulseWidth)
{
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_8*4),ZHighPulseWidth); //pulse_width_high_C
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_9*4),ZLowPulseWidth); //pulse_width_low_C


}
void CncMachine::ReadStatus()
{
	//while(!BITCHECK(IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4),DONE_A));
	//while(!BITCHECK(IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4),DONE_B));
	this->CNC_STATUS.STUS.ULONG = IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4);
}

/////////////////////////////////////////////////////////////////////////////
///@brief toggle run bit to cause axis to move
/////////////////////////////////////////////////////////////////////////////
void CncMachine::MoveZ()
{
	WritePulseInfoZ(this->HighPulseWidthVal,this->LowPulseWidthVal);
	WriteStepNumZ(this->StepNumZ);
	//Toggle run bit
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunZ = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunZ = 0;
	WriteControlRegister();
}

/////////////////////////////////////////////////////////////////////////////
///@brief toggle run bit to cause axis to move
/////////////////////////////////////////////////////////////////////////////
void CncMachine::MoveXY()
{
	//DisplayMovement(GetXYMovement());
	//Toggle run bit
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 1;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 0;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 0;
	WriteControlRegister();
}

/////////////////////////////////////////////////////////////////////////////
///@brief	write high and low pulse width values for the xy-axis then move
///			the xy axis, this is used when manually moving the axis.
/////////////////////////////////////////////////////////////////////////////
void CncMachine::Move()
{
	//write pulse settings to memory mapped RAM
	WriteSettings();
	MoveXY();

}

void CncMachine::DisplayMovement(CncMachine::TRAVERSALXY movement)
{
//	cout<<"########################################"<<endl;
//	cout<<"X_Position= "<<movement.X.Position<<endl;
//	cout<<"X_StepDir = "<<movement.X.StepDir <<endl;
//	cout<<"X_StepNum = "<<movement.X.StepNum <<endl;
//	cout<<endl;
//	cout<<"Y_Position= "<<movement.Y.Position<<endl;
//	cout<<"Y_StepDir = "<<movement.Y.StepDir <<endl;
//	cout<<"Y_StepNum = "<<movement.Y.StepNum <<endl;
//	cout<<"########################################"<<endl;

	printf("########################################\n");
	printf("X_Position= %lu\n",movement.X.Position);
	printf("X_StepDir = %d \n",movement.X.StepDir );
	printf("X_StepNum = %lu\n",movement.X.StepNum );
	printf("X_HighPulseW = %lu\n",movement.X.HighPulseWidth );
	printf("X_LowPulseWidth = %lu\n",movement.X.LowPulseWidth );
	printf("Y_Position= %lu\n",movement.Y.Position);
	printf("Y_StepDir = %d \n",movement.Y.StepDir );
	printf("Y_StepNum = %lu\n",movement.Y.StepNum );
	printf("Y_HighPulseW = %lu\n",movement.Y.HighPulseWidth );
	printf("Y_LowPulseWidth = %lu\n",movement.Y.LowPulseWidth );
	printf("########################################\n");

}

void CncMachine::DisplayRoutes(list<CncMachine::TRAVERSALXY> route_data)
{
	CncMachine::TRAVERSALXY move_to;
	list<CncMachine::TRAVERSALXY>::iterator it;
	for(it = route_data.begin(); it != route_data.end(); it++)
	{
		move_to = *it;
		DisplayMovement(move_to);
	}
}

void CncMachine::ClearRoute()
{
	this->routes.clear();
}
void CncMachine::StartRouting()
{
	this->WriteSettings();
	this->WriteRouterPWM(40000);
	list<CncMachine::TRAVERSALXY> route_data;
	route_data = GetRoutes();
	//DisplayRoutes(route_data);
	CncMachine::TRAVERSALXY movement;
	list<CncMachine::TRAVERSALXY>::iterator it;
	for(it = route_data.begin(); it != route_data.end(); it++)
	{
		movement = *it;
		DisplayMovement(movement);

		WriteStepNum(movement.X.StepNum, movement.Y.StepNum);
		MotorXDir(movement.X.StepDir);
		MotorYDir(movement.Y.StepDir);
		WritePulseInfo(movement.X.HighPulseWidth,movement.X.LowPulseWidth,movement.Y.HighPulseWidth,movement.Y.LowPulseWidth);

		if (movement.router_state == on)
		{
			printf("router on\n");
			//this->WriteRouterPWM(3000);
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser1 = on;
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser2 = on;
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser3 = on;
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser4 = on;
		}
		else if (movement.router_state == off)
		{
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser1 = off;
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser2 = off;
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser3 = off;
			this->CNC_CONTROL.CTRL.CTRL_BITS.Laser4 = off;
			printf("router off\n");
			//this->WriteRouterPWM(1700);
		}
		MoveXY();

		ReadStatus();
		while(!this->CNC_STATUS.STUS.STUS_BITS.XDONE || !this->CNC_STATUS.STUS.STUS_BITS.YDONE)
		{
			//printf("waiting for done\n");

			ReadStatus();

		}

	}
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


