/*
Name: main.cpp
Date: Sep 25, 2014 10:54:09 PM
Author: Mike
Description: firmware that sends pulses to stepper motor based on input data
Copyright: 2014

07/30/2015
mutex for machine route data exist for task 2 and task 3
Info: (CNC_V01.elf) 457 KBytes program size (code + initialized data).
Info:               32311 KBytes free for stack + heap.

*/

#include <list>
#include "types.hpp"

#include "communication_tcp_based.hpp"
//#include "machine_shared_data.hpp"
#include "cncmachine.hpp"


extern "C"
{
#include "stdio.h"
#include "system.h"
#include "includes.h"
}
using namespace std;

/*! Define structure of debugger UART, 16-bit registers on 32-bit boundaries,
 * 	upper words are not used.
 */
struct UART_HW_struct
{
  alt_u16 rxdata;					// Receive Data
  alt_u16 rxdata_unused;
  alt_u16 txdata;					// Transmit Data
  alt_u16 txdata_unused;
  union
  {
    alt_u16 word;
    struct
    {
    	alt_u16  pe         : 1;
    	alt_u16  fe         : 1;
    	alt_u16  brk        : 1;
    	alt_u16  roe        : 1;

    	alt_u16  toe        : 1;
    	alt_u16  tmt        : 1;
    	alt_u16  trdy       : 1;
    	alt_u16  rrdy       : 1;

    	alt_u16  e          : 1;
    	alt_u16  unused     : 1;
    	alt_u16  dcts       : 1;
    	alt_u16  cts        : 1;

    	alt_u16  eop        : 1;
    	alt_u16  reserved   : 3;
    } sBits;
  } sStatus;					// Status Bits
  alt_u16 status_unused;

  union
  {
	  alt_u16 word;
    struct
    {
		  alt_u16  ipe        : 1;
		  alt_u16  ife        : 1;
		  alt_u16  ibrk       : 1;
		  alt_u16  iroe       : 1;

		  alt_u16  itoe       : 1;
		  alt_u16  itmt       : 1;
		  alt_u16  itrdy      : 1;
		  alt_u16  irrdy      : 1;

		  alt_u16  ie         : 1;
		  alt_u16  trbk       : 1;
		  alt_u16  idcts      : 1;
		  alt_u16  rts        : 1;

		  alt_u16  ieop       : 1;
		  alt_u16  reserved   : 3;
    } sBits;
  } sControl;					// Control Bits
  alt_u16 control_unused;

  alt_u16 divisor;
  alt_u16 divisor_unused;
  alt_u16 end_of_packet;
  alt_u16 end_of_packet_unused;
  alt_u16 uart_rx_fifo_used;
  alt_u16 uart_rx_fifo_unused;
  alt_u16 uart_tx_fifo_used;
  alt_u16 uart_tx_fifo_unused;
  alt_u16 gap_timeout_value;
  alt_u16 gap_timeout_unvalue;
  alt_u16 timestamp_value;
  alt_u16 timestamp_unvalue;
};

//protocol_status ps;
// Define pointer to Serial IO
struct UART_HW_struct *pUART;


/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];

//define a mutex
OS_EVENT *global_route_mutex;
OS_EVENT *global_recvr_mutex;

/* Definition of Task Priorities */
//note that MUTEX priority must have a lower number than task priority
#define TASK1_PRIORITY          10
#define TASK2_PRIORITY          9
#define TASK3_PRIORITY          8
#define ROUTE_MUTEX_PRIORITY    7
#define RECVR_MUTEX_PRIORITY    6

list<char> clist;
list<CncMachine::TRAVERSALXY> global_machine_route;


/*!
 * check rrdy pin, if active then save value from
 * rxdata into char list else wait
 */
void task1(void* pdata)
{
//    // wait for xmit ready
//    while(!pUART->sStatus.sBits.trdy)
//      ;
//
//    pUART->txdata = ubChar;
  pUART->sControl.sBits.rts = 1;
  printf("task 1 081015 \n");
  char c;
  list<char> task1_local_recvd;
//  INT8U error_code;
//  BOOLEAN available;
  alt_u16 char_count;
  alt_u16 max_char_cnt_in_fifo = 0;
  alt_u16 loop_count;
  if (pUART->sStatus.sBits.rrdy)
  {
      char_count = pUART->uart_rx_fifo_used;
      printf("will clear %d from rx fifo\n",char_count);
    while (pUART->sStatus.sBits.rrdy) //check data available
    {
       //(alt_u8)(pUART->rxdata);
       printf("%c",(char)(pUART->rxdata));
    }
    printf("cleared rx fifo\n");
  }
  while (1)//reading loop
  {


    //read number of characters are in fifo
	char_count = pUART->uart_rx_fifo_used;
	if (char_count > 0)
	{
	    if (char_count> max_char_cnt_in_fifo)
	    {
	        max_char_cnt_in_fifo = char_count;
	    }
	    //printf("count %d, ",char_count);
	    printf("max count %d\n",max_char_cnt_in_fifo);
	    //push all char into list
	    for (int i = 0; i < char_count; i++)
	    {
	        task1_local_recvd.push_back((alt_u8)(pUART->rxdata));//save data
	    }


//	    if (!task1_local_recvd.empty())
//	    {
	        clist.splice(clist.end(),task1_local_recvd);
	        //printf("total ch %lu \n",clist.size());
//	    }
	}
	else
	{
	    //this delay will determine the loop rate of task1
	    //it determines the interval we want to scan for chars in fifo
	    OSTimeDlyHMSM(0, 0, 0, 10);
	    //printf("max count %d\n",max_char_cnt_in_fifo);
	}

//	if (pUART->sStatus.sBits.rrdy) //check data available
//	{
//	    c = (alt_u8)(pUART->rxdata);
//		//printf("%c",c);
//		//printf("count %d\n",pUART->uart_rx_fifo_used);
//	    task1_local_recvd.push_back(c);//save data
//	}
//	else
//	{
//
////	    if (!task1_local_recvd.empty())
////	    {
////            //OSTimeDlyHMSM(0, 0, 0, 1); //delay 1ms
////            //OSMutexAccept() checks if mutex is available (does not block if not available)
////            //returns 1 if available returns 0 if owned by another task
////            available = OSMutexAccept(global_recvr_mutex, &error_code);
////
////            if(!error_code && available)
////            {
//                if (!task1_local_recvd.empty())
//                {
//                    clist.splice(clist.end(),task1_local_recvd);
//                    printf("total ch %d\n",clist.size());
//                }
////            }
////            else
////            {
////                if (error_code)
////                {
////                    printf("task 1 error getting recvr mutex\n");
////                }
////            }
////            OSMutexPost(global_recvr_mutex);
////	    }
//	}
	loop_count++;
	if(loop_count > 10000)
	{
	    printf("max count %d\n",max_char_cnt_in_fifo);
	    loop_count = 0;
	}
  }
  printf("task 1 stopped!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

//processData
void task2(void* pdata)
{
	printf("task 2 online August 12th 9:47 2015\n");
	CommSimple machine_object;
	list<string> layer1;
	list<string>::iterator it;
	string fld;
	list<char> local_recvd;
	char new_byte;
	INT8U error_code;
	alt_u8 ignore_period = 10;
	alt_u8 comstat;
  while (1)
  {
      if (ignore_period < 10)
      {
          if(!local_recvd.empty())
          {local_recvd.clear();}
          ignore_period++;
          OSTimeDlyHMSM(0, 0, 1, 0);
      }
	  //if local received not empty then read from front and input into machine object
      //else wait interval for mutex then check if clist is empty, if not empty merge with local
	  while(!local_recvd.empty())
	  {

		  //read from front then pop off of front
		  new_byte = local_recvd.front();
		  local_recvd.pop_front();
		  comstat = machine_object.input(new_byte);
		  if ( comstat == ERROR)
		  {
		      printf("comm error %d ...will ignore input for 10 seconds\n", comstat);
		     ignore_period = 0;
		     break;
		  }

	  }
      if (!machine_object.routes.empty())
      {
          //printf("front state %d\n",listener.routes.front().router_state);
          OSMutexPend(global_route_mutex, 0, &error_code);

          //the splice function a.splice(a.end(), b);
          //moves items of B to end of A (emptying B at the same time)
          //this operation is O(1)
          if (!error_code)
          {
              global_machine_route.splice(global_machine_route.end(),machine_object.routes);
              printf("items in global_machine_route:  %lu\n",global_machine_route.size());
          }
          else
          {
              printf("task 2 waited too long\n");
          }

          //copy front into global then pop front off
          //global_machine_state.push_back(listener.machine_state.front());
          //listener.machine_state.pop_front();
          OSMutexPost(global_route_mutex);
      }
//	  else
//	  {
//	      OSMutexPend(global_recvr_mutex, 0, &error_code);
//	      if(!error_code)
//	      {
	          if (!clist.empty())
	          {
	              local_recvd.splice(local_recvd.end(),clist);
	          }
	          else
	          {
	              //this delay will determine how fast a command is processed
	              //from the  first button click
	              OSTimeDlyHMSM(0, 0, 0, 300);
	              printf("task 2 alive\n");

	          }
//	      }
//	      else
//	      {
//	          printf("task 2 waited too long for global_recvr_mutex\n");
//	      }
//	      OSMutexPost(global_recvr_mutex);
//	  }


  }
}

//processData
void task3(void* pdata)
{
  CncMachine cnc_task3;
  INT8U error_code;
  list<CncMachine::TRAVERSALXY> local_route;
  alt_u16 popcorn = 0;
  OSTimeDlyHMSM(0, 0, 3, 0);
  while (1)
  {
    OSMutexPend(global_route_mutex, 0, &error_code);
    if(!error_code)
    {
        if (!global_machine_route.empty())
        {

            //move global_machine_route to end of local_route and empty it at the same time
            //this operation is of O(1) so it is quite fast
            local_route.splice(local_route.end(),global_machine_route);
            OSMutexPost(global_route_mutex); //release mutex
        }
        else
        {
            OSMutexPost(global_route_mutex); //release mutex
            OSTimeDlyHMSM(0, 0, 2, 0);
        }
    }
    else
    {
        printf(" task 3 waited too long\n");
    }


    while(!local_route.empty())
    {
        popcorn++;
        printf("popping one %d, %d left\n",(int)popcorn, (int)local_route.size());
        cnc_task3.ExecuteRouteData(local_route.front());
        //OSTimeDlyHMSM(0, 0, 3, 0);
        local_route.pop_front();
    }

    //printf("task 3 is alive\n");

  }
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{

	//standard UART
	//FIFOED_AVALON_UART_0_BASE
	pUART = (struct UART_HW_struct *)(FIFOED_AVALON_UART_0_BASE | 0x4000000);//(UART_BASE | 0x4000000);
	pUART->sControl.word = 0;

	INT8U err;

	//create mutex for shared memory
	global_route_mutex = OSMutexCreate(ROUTE_MUTEX_PRIORITY, &err);
//	global_recvr_mutex = OSMutexCreate(RECVR_MUTEX_PRIORITY, &err);

	printf("creating tasks 081215\n");
    // Create tasks
    OSTaskCreate(task1, NULL, &task1_stk[TASK_STACKSIZE-1], TASK1_PRIORITY);
    OSTaskCreate(task2, NULL, &task2_stk[TASK_STACKSIZE-1], TASK2_PRIORITY);
    OSTaskCreate(task3, NULL, &task3_stk[TASK_STACKSIZE-1], TASK3_PRIORITY);


//
//  OSTaskCreateExt(task1,NULL, (OS_STK *)(void *)&task1_stk[TASK_STACKSIZE-1], TASK1_PRIORITY,
//		  TASK1_PRIORITY, task1_stk, TASK_STACKSIZE,  NULL, 0);
//
//
//  OSTaskCreateExt(task2,
//                  NULL,
//                  (OS_STK *)(void *)&task2_stk[TASK_STACKSIZE-1],
//                  TASK2_PRIORITY,
//                  TASK2_PRIORITY,
//                  task2_stk,
//                  TASK_STACKSIZE,
//                  NULL,
//                  0);
//
//  OSTaskCreateExt(task3,
//                  NULL,
//                  (OS_STK *)(void *)&task3_stk[TASK_STACKSIZE-1],
//                  TASK3_PRIORITY,
//                  TASK3_PRIORITY,
//                  task3_stk,
//                  TASK_STACKSIZE,
//                  NULL,
//                  0);
  OSStart();
  return 0;
}
