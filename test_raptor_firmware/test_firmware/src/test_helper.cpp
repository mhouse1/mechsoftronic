/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Jul 16, 2015 11:44:46 AM
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////




#include "test_helper.h"

#include <iostream>

/////////////////////////////////////////////////////////////////////////////
/// @brief	This function is used to reverse generate gcode from routed_data
///			generated from CncMachine
/////////////////////////////////////////////////////////////////////////////
void DisplayStepCoordinate(list<CncMachine::TRAVERSALXY> route_data)
{
    printf("test_helper started routing\n");
    //DisplayRoutes(route_data);
    CncMachine::TRAVERSALXY movement;
    list<CncMachine::TRAVERSALXY>::iterator it;

   alt_32 xpos = 0;
   alt_32 ypos= 0;
   for(it = route_data.begin(); it != route_data.end(); it++)
   {

       movement = *it;
       //cout<<"GX"<<xpos<<"GY"<<ypos<<endl;
       switch(movement.router_state)
       {
       case (CncMachine::router_xy):
		   if (movement.X.StepDir)
		   {
			   xpos = xpos + movement.X.StepNum;
		   }
		   else
		   {
			   xpos = xpos - movement.X.StepNum;
		   }
		   if (movement.Y.StepDir)
		   {
			   ypos = ypos + movement.Y.StepNum;
		   }
		   else
		   {
			   ypos = ypos - movement.Y.StepNum;
		   }

		   //cout<<"X"<<movement.X.StepNum<<"Y"<<movement.Y.StepNum<<endl;
		   //printf("G3 X%ld Y%ld\n",xpos, ypos);
		   break;
       case (CncMachine::router_z):
    		   printf("Z step %ld dir %ld\n",movement.X.StepNum, movement.X.StepDir);
       	   break;
       default:
    	   break;
       }
   }

    printf("completed routing\n");
}
