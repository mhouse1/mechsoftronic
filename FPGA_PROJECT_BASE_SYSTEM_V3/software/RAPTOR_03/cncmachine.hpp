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
/////////////////////////////////////////////////////////////////////////////

#ifndef CNCMACHINE_HPP_
#define CNCMACHINE_HPP_

#include <list>

using namespace std;



class CncMachine
{
public:

	enum Peripheral {router_off = 0, router_on, router_up, router_down, router_xy, router_z, cnc_pause,router_unknown};

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
				alt_u32 DirectionX     : 1;//3
				alt_u32 DirectionY     : 1;//4
				alt_u32 DirectionZ     : 1;//5
				alt_u32 Laser1		   : 1;//6
				alt_u32 Laser2		   : 1;//7
				alt_u32 Laser3		   : 1;//8
				alt_u32 Laser4		   : 1;//9
				alt_u32 reserved       : 22;
			}CTRL_BITS;
		}CTRL;
	};

	//this register is meant to be only written to by task2
	//CncRoutePause and CncRouteCancel is routed to status
	//register to allow task 3 to read it
	struct DEBUG_REGISTER
	{
		union
		{
			alt_u32 ULONG;
			struct
			{
				alt_u32 Debug0			: 1;
				alt_u32 Debug1			: 1;
				alt_u32 Debug2			: 1;
				alt_u32 Debug3			: 1;
				alt_u32 Debug4			: 1;
				alt_u32 Debug5			: 1;
				alt_u32 Debug6			: 1;
				alt_u32 Debug7			: 1;
				alt_u32 CncRoutePause	: 1;
				alt_u32 CncRouteCancel	: 1;
				alt_u32 DebugUnused	    : 22;
			}DEBUG_BITS;
		}DEBUG;
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
				alt_u32 CncRouteCancel  : 1;
				alt_u32 CncRoutePause	: 1;
				alt_u32 reserved       : 27;
			}STUS_BITS;
		}STUS;
	};
	struct TRAVERSAL
	{
		alt_u32 StepNum; //number of steps to move
		alt_u32 HighPulseWidth;
		alt_u32 LowPulseWidth;
		alt_u32 StepDir; //direction of movement
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
	DEBUG_REGISTER CNC_DEBUG;
	STATUS_REGISTER CNC_STATUS;


	CncMachine();

	void SetAcceleration(alt_u32 speed_start, alt_u32 speed_change);
	void SetCurrentPosition(alt_u32 x, alt_u32 y); //@todo may not need this func
	alt_u8 SetNextPosition(alt_32 x, alt_32 y);
	alt_u8 SetNextZPosition(alt_32 nextz);
	//alt_u8 RouteCircular(alt_32 endx, alt_32 endy, alt_32 ivar, alt_32 jvar);
	void MotorXDir(alt_u32 x);
	void MotorYDir(alt_u32 y);
	void MotorZDir(alt_u32 z);


	void SetHighPulseWidthMin(alt_u32 width);
	void SetHighPulseWidthVal(alt_u32 width);
	void SetHighPulseWidthMax(alt_u32 width);
	void SetLowPulseWidthMin(alt_u32 width);
	void SetLowPulseWidthVal(alt_u32 width);
	void SetLowPulseWidthMax(alt_u32 width);
	void SetNumberOfStepsX(alt_u32 stepnum);
	void SetNumberOfStepsY(alt_u32 stepnum);
	void SetNumberOfStepsZ(alt_u32 stepnum);
	void MoveXY(void);
	void MoveZ(void);
	void MoveY(void);
	void MoveX(void);
	TRAVERSALXY GetXYMovement(void);
	list<TRAVERSALXY> GetRoutes(void);
	void DisplayMovement(TRAVERSALXY movement);
	void DisplayRoutes(list<TRAVERSALXY> route_data);

	//void StartRouting(void);//@todo remove this, should no longer be used
	void ExecuteRouteData(CncMachine::TRAVERSALXY  route_data);
	void ClearRoute(void);
	void ReadStatus(void);
	void WriteDebugRegister(void);

protected:
	alt_u32 PresentX;
	alt_u32 PresentY;
	alt_u32 PresentZ;
	alt_u32  StepNumX;
	alt_u32  StepNumY;
	alt_u32  StepNumZ;
	alt_u32  PulseWidthZH;
	alt_u32  PulseWidthZL;
	alt_u32  PulseWidthYH;
	alt_u32  PulseWidthYL;
	alt_u32  PulseWidthXH;
	alt_u32  PulseWidthXL;
	alt_u32  FeedRate;
	alt_u16  LayerNumber;
	alt_u16  LayerThickness;

	//protected functions
	void WriteStepNumXY(alt_u32 XSteps, alt_u32 YSteps);
	void WriteStepNumZ(alt_u32 ZSteps);
	void WriteStepNumY(alt_u32 YSteps);
	void WriteStepNumX(alt_u32 XSteps);
	void WriteRouterPWM(alt_u32 PWMVal);
	void WritePulseInfoFeed(alt_u32 value);
	void WritePulseInfoXY(alt_u32 XHighPulseWidth, alt_u32 XLowPulseWidth, alt_u32 YHighPulseWidth, alt_u32 YLowPulseWidth);
	void WritePulseInfoZ(alt_u32 ZHighPulseWidth,alt_u32 ZLowPulseWidth);
	void WritePulseInfoY(alt_u32 YHighPulseWidth, alt_u32 YLowPulseWidth);
	void WritePulseInfoX(alt_u32 XHighPulseWidth, alt_u32 XLowPulseWidth);
	void ClearControlRegister(void);
	void WriteControlRegister(void);

	void RouteXY(TRAVERSALXY movement);
	void RouteZ(TRAVERSALXY movement);
	void AppendStateToRoutes(Peripheral state);
	void CncPauseRouting();
private:

	//Stepper Motor private data
	alt_u32 StepPosX; //step count to indicate how far X moved
	alt_u32 StepPosY; //step count to indicate how far Y moved
	alt_u32 StepPosZ;

	alt_u32 NextX;
	alt_u32 NextY;
	alt_u32 NextZ;
	alt_u32 FullRangeStepCount;
	alt_u32 FullRangeDistance;





};



//OS_EVENT *mem_mutex;

#endif /* CNCMACHINE_HPP_ */
