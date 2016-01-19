/////////////////////////////////////////////////////////////////////////////
///Name: main.cpp
///Date: Sep 25, 2014 10:54:09 PM
///Author: Mike
///Description: firmware that sends pulses to stepper motor based on input data
///Copyright: 2014
///
///07/30/2015
///mutex for machine route data exist for task 2 and task 3
///Info: (CNC_V01.elf) 457 KBytes program size (code + initialized data).
///Info:               32311 KBytes free for stack + heap.
///
/////////////////////////////////////////////////////////////////////////////
#include <list>
#include "types.hpp"

#include "communication_tcp_based.hpp"
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
 *
 */
///FIFOED_AVALON_UART_0_RX_FIFO_SIZE = 8192
///pUART->uart_rx_fifo_used must not exceed FIFOED_AVALON_UART_0_RX_FIFO_SIZE
///if FIFOED_AVALON_UART_0_RX_FIFO_SIZE exceeded then communication error will occur
void task1(void* pdata)
{
  pUART->sControl.sBits.rts = 1;
  printf("task 1 01/18/2015 930\n");
  OSTimeDlyHMSM(0, 0, 2, 0);
  list<char> task1_local_recvd;

  alt_u16 char_count;
  alt_u16 max_char_cnt_in_fifo = 0;
  alt_u16 loop_count;

  //clear any data in the rx fifo
  if (pUART->sStatus.sBits.rrdy)
  {
	  OSTimeDlyHMSM(0, 0, 2, 0);

      char_count = pUART->uart_rx_fifo_used;
      printf("will clear %d from rx fifo\n",char_count);
    while (pUART->sStatus.sBits.rrdy) //check data available
    {
    	//must read it once to remove from FIFO
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
	        printf("max count %d\n",max_char_cnt_in_fifo);

	    }
	    printf("char_count count %d\n",char_count);


        //only add to clist if size of clist is less than a certain amount
        //
	    //push all char into list
	    for (int i = 0; i < char_count; i++)
	    {
	        task1_local_recvd.push_back((alt_u8)(pUART->rxdata));//save data
	    }
	        clist.splice(clist.end(),task1_local_recvd);
	}
	else
	{
	    //this delay will determine the loop rate of task1
	    //it determines the interval we want to scan for chars in fifo
	    OSTimeDlyHMSM(0, 0, 0, 10);
	}

    //if there are 100 or more then tell GUI to stopp sending gcode for a while
    //and keep sending until there are less than 100 in global_route
    //@todo will replace this with more advanced transmit message later
    //if theres a need to send more complex messages
    //if char count is more than 1000 then tell gui to stop sending for a bit
    if (char_count > 1000)
    {
            // wait for xmit ready
            alt_u16 timeout = 0;
            while(!pUART->sStatus.sBits.trdy)
            {
                //poll the transmit ready bit in 1ms intervals
                OSTimeDlyHMSM(0, 0, 0, 1);
                timeout++;
                if (timeout > 100)
                    printf("timed out waiting for tx ready\n");
            }
            pUART->txdata = '1';//tell gui to stop sending
            //@todo even though we tell gui to stop sending
            //we should still keep processing any data gui sends
            //after the stop sending request is received, because although
            //firmware request stop sending data, it is required to process
            //high priority commands.
    }
    else
    {
    	OSTimeDlyHMSM(0, 0, 0, 300);
    	          // wait for xmit ready
    	          alt_u16 timeout = 0;
    	          while(!pUART->sStatus.sBits.trdy)
    	          {
    	              //poll the transmit ready bit in 1ms intervals
    	              OSTimeDlyHMSM(0, 0, 0, 1);
    	              timeout++;
    	              if (timeout > 100)
    	                  printf("timed out waiting for tx ready\n");
    	          }
    	          pUART->txdata = '2';//tell gui to start sending
    }
//	loop_count++;
//	if(loop_count > 10000)
//	{
//	    //printf("max count %d\n",max_char_cnt_in_fifo);
//	    loop_count = 0;
//	}
  }
  printf("task 1 stopped!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	process the transmitted data from GUI. Transmitted data include
///			commands and/or GCode. This task will process gcode point to
///			point coordinates and calculate the number of steps the axes
///			needs to move.
///
/////////////////////////////////////////////////////////////////////////////
void task2(void* pdata)
{
	printf("task 2 online August 12th 1000 2016\n");
	CommSimple machine_object;
	list<string> layer1;
	list<string>::iterator it;
	string fld;
	list<char> local_recvd;
	char new_byte;
	INT8U error_code;
	alt_u8 ignore_period = 10;
	alt_u8 comstat;
	alt_u16 size_of_global_route = 0;
	alt_u16 size_of_local_route = 0;
	alt_u16 processed_count = 0;

  while (1)
  {
	  //ignore_period is enabled when an error in communication is received
      if (ignore_period < 10)
      {
          if(!local_recvd.empty())
          {local_recvd.clear();}
          ignore_period++;
          OSTimeDlyHMSM(0, 0, 1, 0);
      }

	  //prevent while loop from starving other tasks
	  OSTimeDlyHMSM(0, 0, 0, 300);

      //conditions for adding to local communication queue
      if (!clist.empty())
      {
          local_recvd.splice(local_recvd.end(),clist);
      }

	  //if local received not empty and global queue is not over threshold then read
      //from front and input into machine object else wait interval for mutex then
      //check if clist is empty, if not empty merge with local


      size_of_local_route = machine_object.routes.size();
      //tell gui task 2 request stop sending low priority data if over threshold
      //tell gui task 2 says ok to send low priority data is under threshold
      if ( (size_of_local_route > 100) || (local_recvd.size() >500))
      {
          alt_u16 timeout = 0;
          while(!pUART->sStatus.sBits.trdy)
          {
              //poll the transmit ready bit in 1ms intervals
              OSTimeDlyHMSM(0, 0, 0, 1);
              timeout++;
              if (timeout > 100)
                  printf("timed out waiting for tx ready\n");
          }
          pUART->txdata = '3';//tell gui task 2 request stop sending
          printf("*\n");
      }
      else if (size_of_local_route < 100 && (local_recvd.size() < 500) )
      {
          alt_u16 timeout = 0;
          while(!pUART->sStatus.sBits.trdy)
          {
              //poll the transmit ready bit in 1ms intervals
              OSTimeDlyHMSM(0, 0, 0, 1);
              timeout++;
              if (timeout > 100)
                  printf("timed out waiting for tx ready\n");
          }
          pUART->txdata = '4';//tell gui task 2 says okay to send
      }

	  while(!local_recvd.empty() )//&& (machine_object.routes.size() < 100))
	  {

		  processed_count += 1;
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

		  //prevent while loop from starving other tasks
		  OSTimeDlyHMSM(0, 0, 0, 100);
	  }
	  if( processed_count > 0)
	  {
		  printf("processed %d\n", processed_count);
		  processed_count = 0;
	  }
	  //update size of global route
	  OSMutexPend(global_route_mutex, 0, &error_code);
	  size_of_global_route = global_machine_route.size();
	  OSMutexPost(global_route_mutex);


	  //conditions for adding to global route queue
	  //in the statement if (!machine_object.routes.empty() && (size_of_global_route < x))
	  //'x' is one of the variables that will affect how long until
	  //the high priority communication will get processed
      if ((size_of_global_route < 100) && !machine_object.routes.empty() )
      {
    	  //wait for mutex
          OSMutexPend(global_route_mutex, 0, &error_code);

          //the splice function a.splice(a.end(), b);
          //moves items of B to end of A (emptying B at the same time)
          //this operation is O(1)
          if (!error_code)
          {
              global_machine_route.splice(global_machine_route.end(),machine_object.routes);
          }
          else
          {
              printf("task 2 waited too long\n");
          }

          //copy front into global then pop front off
          OSMutexPost(global_route_mutex);
      }
  }
}

/////////////////////////////////////////////////////////////////////////////
///@brief 	set the acceleration data
/////////////////////////////////////////////////////////////////////////////
void task3(void* pdata)
{
  CncMachine cnc_task3;
  INT8U error_code;
  list<CncMachine::TRAVERSALXY> local_route;
  alt_u16 popcorn = 0;
  OSTimeDlyHMSM(0, 0, 3, 0);
  cnc_task3.CNC_DEBUG.DEBUG.ULONG = 0;
  cnc_task3.WriteDebugRegister();
  while (1)
  {
    OSMutexPend(global_route_mutex, 0, &error_code);
    if(!error_code)
    {
    	//conditions for adding to local route
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
            //printf("task3!\n");
            OSTimeDlyHMSM(0, 0, 2, 0);
        }
    }
    else
    {
        printf(" task 3 waited too long\n");
    }


    //execute route until local route is empty
    while(!local_route.empty())
    {
    	//this delay is required so other task does not block for long
    	//periods of time, this delay also needs to be as small as possible
    	//so it seems like transitions from point to point instantaneous
    	//OSTimeDlyHMSM(0, 0, 0, 2);
    	cnc_task3.ReadStatus();
    	if(cnc_task3.CNC_STATUS.STUS.STUS_BITS.XDONE &&
    	   cnc_task3.CNC_STATUS.STUS.STUS_BITS.YDONE &&
    	   cnc_task3.CNC_STATUS.STUS.STUS_BITS.ZDONE &&
    	   !cnc_task3.CNC_STATUS.STUS.STUS_BITS.CncRoutePause &&
    	   !cnc_task3.CNC_STATUS.STUS.STUS_BITS.CncRouteCancel)
		{
			popcorn++;
			//printf("popping one %d, %d left\n",(int)popcorn, (int)local_route.size());
			cnc_task3.ExecuteRouteData(local_route.front());
			local_route.pop_front();
			OSTimeDlyHMSM(0, 0, 0, 2);//polling interval to check if done routing
		}


    	//wait until stepping is done or until pause not active
    	while(!cnc_task3.CNC_STATUS.STUS.STUS_BITS.XDONE ||
    		  !cnc_task3.CNC_STATUS.STUS.STUS_BITS.YDONE ||
    		  !cnc_task3.CNC_STATUS.STUS.STUS_BITS.ZDONE ||
    		  cnc_task3.CNC_STATUS.STUS.STUS_BITS.CncRoutePause ||
    		  cnc_task3.CNC_STATUS.STUS.STUS_BITS.CncRouteCancel)
    	{
    		OSTimeDlyHMSM(0, 0, 0, 2);
        	if (cnc_task3.CNC_STATUS.STUS.STUS_BITS.CncRouteCancel)
        	{
        		local_route.clear();
        		break;
        	}

        	cnc_task3.ReadStatus();
    	}
    }
    //printf("task 3 is alive\n");

  }
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
	pUART = (struct UART_HW_struct *)(FIFOED_AVALON_UART_0_BASE | 0x4000000);
	pUART->sControl.word = 0;

	INT8U err;

	//create mutex for shared memory
	global_route_mutex = OSMutexCreate(ROUTE_MUTEX_PRIORITY, &err);
//	global_recvr_mutex = OSMutexCreate(RECVR_MUTEX_PRIORITY, &err);

	printf("creating tasks 909 \n");
    // Create tasks
    OSTaskCreate(task1, NULL, &task1_stk[TASK_STACKSIZE-1], TASK1_PRIORITY);
    OSTaskCreate(task2, NULL, &task2_stk[TASK_STACKSIZE-1], TASK2_PRIORITY);
    OSTaskCreate(task3, NULL, &task3_stk[TASK_STACKSIZE-1], TASK3_PRIORITY);

    OSStart();
    return 0;
}
