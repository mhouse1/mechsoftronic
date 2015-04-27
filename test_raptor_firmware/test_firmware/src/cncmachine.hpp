/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Sep 19, 2014 12:57:19 PM
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

#ifndef CNCMACHINE_HPP_
#define CNCMACHINE_HPP_

#include <list>

using namespace std;
class CncMachine
{
public:

	enum Direction {down = 0,up};
	enum Peripheral {off = 0, on};

	//This struct has the exact mapping as defined in the VHDL file
	//the control_register only contains the bits to signal start
	//moving an axis, changing the direction of an axis
	//and bits to toggle laser on or off
	struct CONTROL_REGISTER
	{
		union
		{
			alt_u32 ULONG;
			struct
			{
				alt_u32 RunX           : 1;//0
				alt_u32 RunY           : 1;//1
				alt_u32 RunZ           : 1;//2
				Direction DirectionX   : 1;//3
				Direction DirectionY   : 1;//4
				Direction DirectionZ   : 1;//5
				alt_u32 Laser1		   : 1;//6
				alt_u32 Laser2		   : 1;//7
				alt_u32 Laser3		   : 1;//8
				alt_u32 Laser4		   : 1;//9
				alt_u32 reserved       : 22;
			}CTRL_BITS;
		}CTRL;
	};

	struct STATUS_REGISTER
	{
		union
		{
			alt_u32 ULONG;
			struct
			{
				alt_u32 XDONE           : 1;
				alt_u32 YDONE           : 1;
				alt_u32 ZDONE			: 1;
				alt_u32 reserved       : 29;
			}STUS_BITS;
		}STUS;
	};
	struct TRAVERSAL
	{
		alt_u32 StepNum; //number of steps to move
		alt_u32 HighPulseWidth;
		alt_u32 LowPulseWidth;
		Direction StepDir; //direction of movement
		alt_u32 Position;//current position on table
	};

	Peripheral router_state;

	struct TRAVERSALXY
	{
		TRAVERSAL X;
		TRAVERSAL Y;
		Peripheral router_state;
	};
	//Public data

	list<TRAVERSALXY> routes;
	CONTROL_REGISTER CNC_CONTROL;
	STATUS_REGISTER CNC_STATUS;


	CncMachine();

	void SetCurrentPosition(alt_u32 x, alt_u32 y); //@todo may not need this func
	alt_u8 SetNextPosition(alt_u32 x, alt_u32 y);
	void MotorXDir(Direction x);
	void MotorYDir(Direction y);
	void MotorZDir(Direction z);

	void SetMaxSpeed(alt_u32 speed);
	void SetSpeedVal(alt_u32 speed);
	void SetHighPulseWidthMin(alt_u32 width);
	void SetHighPulseWidthVal(alt_u32 width);
	void SetHighPulseWidthMax(alt_u32 width);
	void SetLowPulseWidthMin(alt_u32 width);
	void SetLowPulseWidthVal(alt_u32 width);
	void SetLowPulseWidthMax(alt_u32 width);
	void SetNumberOfStepsX(alt_u32 stepnum);
	void SetNumberOfStepsY(alt_u32 stepnum);
	void SetNumberOfStepsZ(alt_u32 stepnum);
	void Move(void);
	void MoveXY(void);
	void MoveZ(void);
	TRAVERSALXY GetXYMovement(void);
	list<TRAVERSALXY> GetRoutes(void);
	void DisplayMovement(TRAVERSALXY movement);
	void DisplayRoutes(list<TRAVERSALXY> route_data);

	void StartRouting(void);
	void ClearRoute(void);
	void ReadStatus(void);

protected:
	//Private functions
	void WriteStepNum(alt_u32 XSteps, alt_u32 YSteps);
	void WriteStepNumZ(alt_u32 ZSteps);
	void WriteRouterPWM(alt_u32 PWMVal);
	void WriteSettings(void);
	void WritePulseInfo(alt_u32 XHighPulseWidth, alt_u32 XLowPulseWidth, alt_u32 YHighPulseWidth, alt_u32 YLowPulseWidth);
	void WritePulseInfoZ(alt_u32 ZHighPulseWidth,alt_u32 ZLowPulseWidth);
	void ClearControlRegister(void);
	void WriteControlRegister(void);
private:

	//Stepper Motor private data
	alt_u32 StepPosX; //step count to indicate how far X moved
	alt_u32 StepPosY; //step count to indicate how far Y moved
	alt_u32 StepPosZ;
	alt_u32 PresentX;
	alt_u32 PresentY;
	alt_u32 PresentZ;
	alt_u32 NextX;
	alt_u32 NextY;
	alt_u32 NextZ;
	alt_32  StepNumX;
	alt_32  StepNumY;
	alt_32  StepNumZ;
	alt_u32 MaxSpeed;
	alt_u32 SpeedVal;
	alt_u32 HighPulseWidthMin;
	alt_u32 HighPulseWidthVal;
	alt_u32 HighPulseWidthMax;
	alt_u32 LowPulseWidthMin;
	alt_u32 LowPulseWidthVal;
	alt_u32 LowPulseWidthMax;

	alt_u32 FullRangeStepCount;
	alt_u32 FullRangeDistance;


};




#endif /* CNCMACHINE_HPP_ */
