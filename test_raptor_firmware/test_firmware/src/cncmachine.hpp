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
#include <list>

#ifndef CNCMACHINE_HPP_
#define CNCMACHINE_HPP_


class CncMachine
{
public:

	enum Direction {down = 0,up};
	enum Peripheral {on = 0, off};

	struct CONTROL_REGISTER
	{
		union
		{
			alt_u32 ULONG;
			struct
			{
				alt_u32 RunX           : 1;
				alt_u32 RunY           : 1;
				Direction DirectionX   : 1;
				Direction DirectionY   : 1;
				alt_u32 reserved       : 28;
			}CTRL_BITS;
		}CTRL;
	};

	struct TRAVERSAL
	{
		alt_u32 StepNum; //number of steps to move
		Direction StepDir; //direction of movement
		alt_u32 Position;//current position on table
	};

	struct TRAVERSALXY
	{
		TRAVERSAL X;
		TRAVERSAL Y;
		Peripheral cutter_state;
	};
	//Public data

	std::list<TRAVERSALXY> route;
	CONTROL_REGISTER CNC_CONTROL;


	CncMachine();

	void SetCurrentPosition(alt_u32 x, alt_u32 y); //@todo may not need this func
	alt_u8 SetNextPosition(alt_u32 x, alt_u32 y);
	void MotorXDir(Direction x);
	void MotorYDir(Direction y);

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
	void Move();
	TRAVERSALXY GetXYMovement(void);
protected:
	//Private functions
	void WriteSettings(void);
	void ClearControlRegister(void);
	void WriteControlRegister(void);

private:

	//Stepper Motor private data
	alt_u32 StepPosX; //step count to indicate how far X moved
	alt_u32 StepPosY; //step count to indicate how far Y moved
	alt_u32 PresentX;
	alt_u32 PresentY;
	alt_u32 NextX;
	alt_u32 NextY;
	alt_32  StepNumX;
	alt_32  StepNumY;
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
