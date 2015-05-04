/*
Name: main.cpp
Date: Sep 25, 2014 10:54:09 PM
Author: Mike
Description: firmware that sends pulses to stepper motor based on input data

Copyright: 2014
*/

#include <list>
#include "types.hpp"
#include "cncmachine.hpp"
#include "communication_tcp_based.hpp"

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
};

//protocol_status ps;
// Define pointer to Serial IO
struct UART_HW_struct *pUART;


/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

list<char> clist;
/*!
 * check rrdy pin, if active then save value from
 * rxdata into char list else wait
 */
void task1(void* pdata)
{
  pUART->sControl.sBits.rts = 1;
  printf("task 1 initialized\n");
  char c;
  while (1)//reading loop
  {
	if (pUART->sStatus.sBits.rrdy) //check data available
	{	c = (alt_u8)(pUART->rxdata);
		printf("%c",c);
		clist.push_back(c);//save data
	}
	else //delay
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //delay 1ms
	}
  }
}
/* Prints "Hello World" and sleeps for three seconds */
//processData
void task2(void* pdata)
{
	printf("Data processor online April 28th 2015\n");
//	ProtocolWrapper pw;
//	protocol_status ps;
//	PsychoFrameHeart psycommu;
	CommSimple listener;
	list<string> fields_list;
	list<string> layer1;
	list<string>::iterator it;
	string fld;

	char new_byte;
  while (1)
  {
	  //if receiver not in ready state and data queue is not empty
	  if(!pUART->sStatus.sBits.rrdy && !clist.empty())
	  {
		  //read from front then pop off of front
		  new_byte = clist.front();
		  clist.pop_front();

		  listener.input(new_byte);

	  }
	  else
	  {
		  OSTimeDlyHMSM(0, 0, 0, 300);
	  }

//	//size = clist.size();
//	if (!pUART->sStatus.sBits.rrdy)
//	{
//	if (!clist.empty())
//	{
//		c = clist.front();//read front item
//		clist.pop_front();//remove front item
//		ps = (protocol_status) pw.input(c);
//		//printf("   protocol status = %d\n",ps);
//		if(ps == MSG_OK)
//		{
//
//			//get fields layer 1
//			fields_list = pw.get_fields();
//			printf("last message %s \n", pw.last_message.c_str());
//
//			//process fields_list
//			try
//			{
//				if (!(psycommu.input(pw.get_lastmessage()) == 0))
//				{
//					printf("error non zero exit for psycommu\n");
//				}
//			}
//			catch(...)
//			{
//				printf("exception processing input\n");
//			}
//		}
//
//	}
//	else
//	{
//    OSTimeDlyHMSM(0, 0, 1, 0);
//    //printf("\n");
//	}
//	}
//	else
//	{
//    OSTimeDlyHMSM(0, 0, 1, 0);
//    //printf("\n");
//	}
  }
}
/* The main function creates two task and starts multi-tasking */
int main(void)
{

	//standard UART
	pUART = (struct UART_HW_struct *)(UART_BASE | 0x4000000);
	pUART->sControl.word = 0;

  OSTaskCreateExt(task1,NULL, (OS_STK *)(void *)&task1_stk[TASK_STACKSIZE-1], TASK1_PRIORITY,
		  TASK1_PRIORITY, task1_stk, TASK_STACKSIZE,  NULL, 0);


  OSTaskCreateExt(task2,
                  NULL,
                  (OS_STK *)(void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
  OSStart();
  return 0;
}
