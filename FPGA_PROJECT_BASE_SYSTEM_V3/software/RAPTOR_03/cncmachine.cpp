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
#include <iostream>
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
	this->LayerNumber		= 1;
	this->LayerThickness	= 0;

	this->PulseWidthZH      = 20000;
	this->PulseWidthZL      = 3000;
	this->PulseWidthYH      = 20000;
	this->PulseWidthYL      = 3000;
	this->PulseWidthXH      = 20000;
	this->PulseWidthXL      = 3000;
	this->FeedRate			= 200000;
	this->router_state = router_off;
	this->FullRangeStepCount= 2000;
	this->FullRangeDistance = 380000; //38mm scaled by 10000
	this->CNC_CONTROL.CTRL.ULONG  = 0;
	this->CNC_STATUS.STUS.ULONG = 0;


	//this->ControlRegister	= 0;
}

/////////////////////////////////////////////////////////////////////////////
///@brief This Section is dedicated to Set functions
/////////////////////////////////////////////////////////////////////////////
void CncMachine::SetAcceleration(alt_u32 speed_start, alt_u32 speed_change)
{
	printf("acceleration set\n");
	printf("speed_start = %lu\n",speed_start);
	printf("speed_change = %lu\n",speed_change);

	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_11*4),speed_start);
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_12*4),speed_change);
}



/////////////////////////////////////////////////////////////////////////////
///@brief This Section is dedicated to Set functions
/////////////////////////////////////////////////////////////////////////////
void CncMachine::SetCurrentPosition(alt_u32 x, alt_u32 y)
{
	this->PresentX = x;
	this->PresentY = y;
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

void CncMachine::WriteStepNumXY(alt_u32 XSteps, alt_u32 YSteps)
{
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_2*4),XSteps );
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_6*4),YSteps );
}

void CncMachine::WriteStepNumX(alt_u32 XSteps)
{
	IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_2*4),XSteps );
}

void CncMachine::WriteStepNumY(alt_u32 YSteps)
{
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
///@brief Write pulse info for x and y axis
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfoXY(alt_u32 XHighPulseWidth, alt_u32 XLowPulseWidth, alt_u32 YHighPulseWidth, alt_u32 YLowPulseWidth)
{
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_0*4),XHighPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_1*4),XLowPulseWidth);

   //IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_3*4),0); //Control register
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_4*4),YHighPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_5*4),YLowPulseWidth );
	 //WriteStepNum(this->StepNumX, this->StepNumY);

}


/////////////////////////////////////////////////////////////////////////////
///@brief Write pulse info for z axis
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfoZ(alt_u32 ZHighPulseWidth,alt_u32 ZLowPulseWidth)
{
	printf("set Z High Pulse Width Min = %lu\n",ZHighPulseWidth);
	printf("set Z High Pulse Width Min = %lu\n",ZLowPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_8*4),ZHighPulseWidth); //pulse_width_high_C
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_9*4),ZLowPulseWidth); //pulse_width_low_C


}

/////////////////////////////////////////////////////////////////////////////
///@brief Write pulse info for y axis
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfoY(alt_u32 YHighPulseWidth, alt_u32 YLowPulseWidth)
{
	printf("set Y High Pulse Width Min = %lu\n",YHighPulseWidth);
	printf("set Y High Pulse Width Min = %lu\n",YLowPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_4*4),YHighPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_5*4),YLowPulseWidth );
}


/////////////////////////////////////////////////////////////////////////////
///@brief Write pulse info for x axis
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfoX(alt_u32 XHighPulseWidth, alt_u32 XLowPulseWidth)
{
		printf("set X High Pulse Width Min = %lu\n",XHighPulseWidth);
		printf("set X High Pulse Width Min = %lu\n",XLowPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_0*4),XHighPulseWidth);
	 IOWR_32DIRECT(SLAVE_TEMPLATE_1_BASE,(DATA_OUT_1*4),XLowPulseWidth);
}

/////////////////////////////////////////////////////////////////////////////
///@brief Write pulse info for base feed rate
///		  routing speeds has a base speed based off of High and L values
///		  cutting (routing) speed and traveling speed may have different speeds
///	      typically cutting speed will be slower than traveling
/////////////////////////////////////////////////////////////////////////////
void CncMachine::WritePulseInfoFeed(alt_u32 value)
{
	printf("set feed High Pulse Width  = %lu\n",value);
	this->FeedRate			= value;
}


void CncMachine::ReadStatus()
{
	//while(!BITCHECK(IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4),DONE_A));
	//while(!BITCHECK(IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4),DONE_B));
	this->CNC_STATUS.STUS.ULONG = IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4);
}

/////////////////////////////////////////////////////////////////////////////
///@brief toggle run bit to cause axis to move
///
///@todo	modify function to accept parameter for direction, and stepnum
///			since everytime we move the axis those things need to be set
/////////////////////////////////////////////////////////////////////////////
void CncMachine::MoveZ()
{
	WriteStepNumZ(this->StepNumZ);
	//Toggle run bit
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunZ = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunZ = 0;
	WriteControlRegister();
	printf("Jog z axis in cnc machine %lu\n",this->StepNumZ);
}

/////////////////////////////////////////////////////////////////////////////
///@brief toggle run bit to cause axis to move
/////////////////////////////////////////////////////////////////////////////
void CncMachine::MoveY()
{
	WriteStepNumY(this->StepNumY);
	//Toggle run bit
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 0;
	WriteControlRegister();
	printf("Jog y axis in cnc machine %lu\n",this->StepNumY);
}

/////////////////////////////////////////////////////////////////////////////
///@brief toggle run bit to cause axis to move
/////////////////////////////////////////////////////////////////////////////
void CncMachine::MoveX()
{
	WriteStepNumX(this->StepNumX);
	//Toggle run bit
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 0;
	WriteControlRegister();
	printf("Jog x axis in cnc machine %lu\n",this->StepNumX);
}
/////////////////////////////////////////////////////////////////////////////
///@brief toggle run bit to cause axis to move
/////////////////////////////////////////////////////////////////////////////
void CncMachine::MoveXY()
{
	//DisplayMovement(GetXYMovement());
	//Toggle run bit
	WriteStepNumXY(this->StepNumX,this->StepNumY);
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 1;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 0;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 0;
	WriteControlRegister();
	printf("Jog xy axis in cnc machine\n");
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
///@todo	right now itll use pulse info for x axis as speed base
///			will review this later
/////////////////////////////////////////////////////////////////////////////
alt_u8 CncMachine::SetNextPosition(alt_u32 x, alt_u32 y)
{
	TRAVERSALXY data;

	//assign state router_xy to indicate data is xy movement
	data.router_state = router_xy;
	data.X.Position = x;
	data.Y.Position = y;

	//Convert distance to steps
	//calculate distance then set the new position as present position
	alt_32 distanceX = x - this->PresentX;
	alt_32 distanceY = y - this->PresentY;
	this->PresentX = x;
	this->PresentY = y;
	//calculate and set direction
	//if change in distance is positive move up, else move down
	data.X.StepDir = distanceX >= 0? up:down;
	data.Y.StepDir = distanceY >= 0? up:down;

	//05/04/2015 for now just calculate number of steps to take based on distance divided by some number
	//			 @todo will need to figure out the actual number of steps per distance increment later to plot accurately
	//05/09/2015 measured 1000 steps to move 22mm equivalent of 44.45 pulses per mm.
	//			 the received distanceXY is scaled up by a number in the GUI,
	//			 the actual StepNum should be StepNum = (distanceX*44.45)/GUI_Scaling
	//			 in instruction terms this would be (distanceX/220)
	//@todo add gui scaling parameter
	data.X.StepNum = (alt_32)fabs(distanceX)/221;//(this->FullRangeStepCount*(alt_32)fabs(distanceX))/this->FullRangeDistance;
	data.Y.StepNum = (alt_32)fabs(distanceY)/221;//(this->FullRangeStepCount*(alt_32)fabs(distanceY))/this->FullRangeDistance;

	//for now use x axis pulse width info for base speed (pulse width counts)
	alt_u32 basePWH = this->FeedRate;
	alt_u32 basePWL = this->FeedRate;

	//calculate the base pulse period divided by 2
	alt_u32 HalfBasePulsePeriod = (basePWH + basePWL)/2;


	//slow down the axis with less steps else uses base Pulse width info for both axis
	if (data.X.StepNum > data.Y.StepNum) //X has more steps to take than y, so slow down the Y axis
	{
		//check if Y axis is going to move at all
		// if Y axis has steps to move then slow down the Y axis movement
		// so it finishes at the same time as X axis
		if (data.Y.StepNum != 0)
		{
			//use base pulse width for x axis
			data.X.HighPulseWidth = basePWH;
			data.X.LowPulseWidth = basePWL;

			//calculate the base pulse widths for Y axis
			//so Y axis completes at the same time as x axis
			//use the same
			alt_u32 totalTimeXhalf = data.X.StepNum * HalfBasePulsePeriod;
			alt_u32 totalTimeYhalf = totalTimeXhalf/data.Y.StepNum;

			data.Y.HighPulseWidth = totalTimeYhalf;
			data.Y.LowPulseWidth = totalTimeYhalf;

		}
		else //Y.StepNum = 0 (Y axis does not move)
		{
			//set XY pulse width info to base value
			data.X.HighPulseWidth = basePWH;
			data.X.LowPulseWidth = basePWL;
			data.Y.HighPulseWidth = basePWH;
			data.Y.LowPulseWidth = basePWL;
		}
	}

	else if  (data.Y.StepNum > data.X.StepNum) //Y has more steps to take than X
	{
		if (data.X.StepNum != 0)
		{
			data.Y.HighPulseWidth = basePWH;
			data.Y.LowPulseWidth = basePWL;

			alt_u32 totalTimeYhalf = data.Y.StepNum * HalfBasePulsePeriod;
			alt_u32 totalTimeXhalf = totalTimeYhalf/data.X.StepNum;

			data.X.HighPulseWidth = totalTimeXhalf;
			data.X.LowPulseWidth = totalTimeXhalf;

		}
		else //X.StepNum = 0 (X axis does not move)
		{
			data.X.HighPulseWidth = basePWH;
			data.X.LowPulseWidth = basePWL;
			data.Y.HighPulseWidth = basePWH;
			data.Y.LowPulseWidth = basePWL;
		}
	}
	else //Y and X has equal number of steps to take
	{
		data.X.HighPulseWidth = basePWH;
		data.X.LowPulseWidth = basePWL;
		data.Y.HighPulseWidth = basePWH;
		data.Y.LowPulseWidth = basePWL;
	}



	this->routes.push_back(data);
	return 0;
}

void CncMachine::AppendStateToRoutes(Peripheral state)
{
	TRAVERSALXY data;
	data.router_state = state;
	this->routes.push_back(data);
}
void CncMachine::RouteXY(TRAVERSALXY movement)
{
	DisplayMovement(movement);

	WriteStepNumXY(movement.X.StepNum, movement.Y.StepNum);
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionX = movement.X.StepDir;
	this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionY = movement.Y.StepDir;
	WritePulseInfoXY(movement.X.HighPulseWidth,movement.X.LowPulseWidth,movement.Y.HighPulseWidth,movement.Y.LowPulseWidth);

//	if (movement.router_state == router_on)
//	{
//		printf("router on\n");
//		//this->WriteRouterPWM(3000);
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser1 = on;
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser2 = on;
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser3 = on;
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser4 = on;
//	}
//	else if (movement.router_state == router_off)
//	{
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser1 = off;
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser2 = off;
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser3 = off;
////				this->CNC_CONTROL.CTRL.CTRL_BITS.Laser4 = off;
//		printf("router off\n");
//		//this->WriteRouterPWM(1700);
//	}

	//run x and y axis
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 1;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 1;
	WriteControlRegister();
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunX = 0;
	this->CNC_CONTROL.CTRL.CTRL_BITS.RunY = 0;
	WriteControlRegister();

	//wait until stepping is done
	ReadStatus();
	while(!this->CNC_STATUS.STUS.STUS_BITS.XDONE || !this->CNC_STATUS.STUS.STUS_BITS.YDONE)
	{
		ReadStatus();
	}
}

void CncMachine::StartRouting()
{
	printf("started routing\n");
	this->WriteRouterPWM(40000);
	list<CncMachine::TRAVERSALXY> route_data;
	route_data = this->routes;
	//DisplayRoutes(route_data);
	CncMachine::TRAVERSALXY movement;
	list<CncMachine::TRAVERSALXY>::iterator it;

	alt_u16 layer_count;

	for(layer_count = 0; layer_count< this->LayerNumber; layer_count++)
	{
		this->CNC_CONTROL.CTRL.CTRL_BITS.DirectionZ = down;
		this->StepNumZ = this->LayerThickness;
		this->MoveZ();

		for(it = route_data.begin(); it != route_data.end(); it++)
		{
			movement = *it;
			switch(movement.router_state)
			{
			case(router_on):
				//turn router on
				break;
			case(router_off):
				//turn router off
				break;
			case(router_xy):
				//move xy
				RouteXY(movement);
				break;
			default:
				printf("undefined router state");
				break;
			}

		}
	}
	printf("completed routing\n");
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


