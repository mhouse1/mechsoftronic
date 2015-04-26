#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

#include <list>
#include "types.hpp"
#include "cncmachine.hpp"
#include "protocolwrapper.hpp"
#include "psychoframeheart.hpp"
extern "C"
{
#include "stdio.h"
}
using namespace std;

//#include <list>
//#include "types.hpp"
//#include "cncmachine.hpp"
//#include "protocolwrapper.hpp"
//#include "psychoframeheart.hpp"
//extern "C"
//{
//#include "stdio.h"
//#include "system.h"
//#include "includes.h"
//}
//using namespace std;

//using namespace std;
//
///*! Define structure of debugger UART, 16-bit registers on 32-bit boundaries,
// * 	upper words are not used.
// */
//struct UART_HW_struct
//{
//  alt_u16 rxdata;					// Receive Data
//  alt_u16 rxdata_unused;
//  alt_u16 txdata;					// Transmit Data
//  alt_u16 txdata_unused;
//  union
//  {
//    alt_u16 word;
//    struct
//    {
//    	alt_u16  pe         : 1;
//    	alt_u16  fe         : 1;
//    	alt_u16  brk        : 1;
//    	alt_u16  roe        : 1;
//
//    	alt_u16  toe        : 1;
//    	alt_u16  tmt        : 1;
//    	alt_u16  trdy       : 1;
//    	alt_u16  rrdy       : 1;
//
//    	alt_u16  e          : 1;
//    	alt_u16  unused     : 1;
//    	alt_u16  dcts       : 1;
//    	alt_u16  cts        : 1;
//
//    	alt_u16  eop        : 1;
//    	alt_u16  reserved   : 3;
//    } sBits;
//  } sStatus;					// Status Bits
//  alt_u16 status_unused;
//
//  union
//  {
//	  alt_u16 word;
//    struct
//    {
//		  alt_u16  ipe        : 1;
//		  alt_u16  ife        : 1;
//		  alt_u16  ibrk       : 1;
//		  alt_u16  iroe       : 1;
//
//		  alt_u16  itoe       : 1;
//		  alt_u16  itmt       : 1;
//		  alt_u16  itrdy      : 1;
//		  alt_u16  irrdy      : 1;
//
//		  alt_u16  ie         : 1;
//		  alt_u16  trbk       : 1;
//		  alt_u16  idcts      : 1;
//		  alt_u16  rts        : 1;
//
//		  alt_u16  ieop       : 1;
//		  alt_u16  reserved   : 3;
//    } sBits;
//  } sControl;					// Control Bits
//  alt_u16 control_unused;
//
//  alt_u16 divisor;
//  alt_u16 divisor_unused;
//  alt_u16 end_of_packet;
//  alt_u16 end_of_packet_unused;
//};
//
//protocol_status ps;
//// Define pointer to Serial IO
//struct UART_HW_struct *pUART;
//
//
///* Definition of Task Stacks */
//#define   TASK_STACKSIZE       2048
//OS_STK    task1_stk[TASK_STACKSIZE];
//OS_STK    task2_stk[TASK_STACKSIZE];
//
///* Definition of Task Priorities */
//#define TASK1_PRIORITY      1
//#define TASK2_PRIORITY      2
//
//list<char> clist;
///*!
// * check rrdy pin, if active then save value from
// * rxdata into char list else wait
// */
//void task1(void* pdata)
//{
//  pUART = (struct UART_HW_struct *)(UART_BASE | 0x4000000);
//  pUART->sControl.word = 0;
//  pUART->sControl.sBits.rts = 1;
//  printf("task 1 initialized\n");
//  char c;
//  while (1)//reading loop
//  {
//	if (pUART->sStatus.sBits.rrdy) //check data available
//	{	c = (alt_u8)(pUART->rxdata);
//		//printf("%c",c);
//		clist.push_back(c);//save data
//	}
//	else //delay
//	{
//		OSTimeDlyHMSM(0, 0, 0, 1); //delay 1ms
//	}
//  }
//}
///* Prints "Hello World" and sleeps for three seconds */
////processData
//void task2(void* pdata)
//{
//	printf("hello task 2\n");
//	ProtocolWrapper pw;
//	protocol_status ps;
//	list<string> fields_list;
//	list<string> layer1;
//	list<string>::iterator it;
//	string fld;
//	PsychoFrameHeart psycommu;
//	char c;
//	printf("started processing data ...\n");
//  while (1)
//  {
//	//size = clist.size();
//	if (!clist.empty())
//	{
//		c = clist.front();//read front item
//		clist.pop_front();//remove front item
//		ps = (protocol_status) pw.input(c);
//		//printf("   protocol status = %d\n",ps);
//		if(ps == MSG_OK)
//		{
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
//	}
//	else
//	{
//    OSTimeDlyHMSM(0, 0, 1, 0);
//    //printf("\n");
//	}
//  }
//}
///* The main function creates two task and starts multi-tasking */
//int main(void)
//{
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
//  OSStart();
//  return 0;
//}


//void printFields(list<string> fields_list);
//


/////////////////////////////////////////////////////////////////////////////
///@brief the firmware on de-0 nano will implement a version of this function
///		  to process data received from the serial port
/////////////////////////////////////////////////////////////////////////////
void processData(list<char> clist)
{
	int size;
	ProtocolWrapper pw;
	protocol_status ps;

	list<string> fields_list;
	list<string> layer1;
	list<string>::iterator it;
	string fld;
	PsychoFrameHeart psycommu;
	char c;
	printf("started processing data ...\n");
	  while (!clist.empty())
	  {
		size = clist.size();
		//printf("number of characters in clist = %d ",size);

		if (!clist.empty())
		{
			c = clist.front();//read front item
			clist.pop_front();//remove front item
			ps = (protocol_status) pw.input(c);
			//printf("   protocol status = %d\n",ps);
			if(ps == MSG_OK)
			{
				//get fields layer 1
				fields_list = pw.get_fields();
				//printf("last message %s \n", pw.last_message.c_str());

				//process fields_list
				try
				{
					if (!(psycommu.input(pw.get_lastmessage()) == 0))
					{
						printf("error non zero exit for psycommu\n");
					}
				}
				catch(...)
				{
					printf("exception processing input\n");
				}
			}
		}
		else
		{
			printf("empty list\n");
			//break;
	    //OSTimeDlyHMSM(0, 0, 1, 0);
		}
	  }
	  printf("finished processing data ...\n");
}

list<char> buildCharList(list<string> slist)
{
	list<char> char_list;
	list<string>::iterator itd;

	for(itd = slist.begin(); itd != slist.end(); itd++)
	{
		string td_string = (string)*itd;
		//iterate through char in string, append to clist
		for(unsigned int i = 0; i< td_string.length(); i++)
		{
			//char c = td_string[i];
			char_list.push_back(td_string[i]);
		}
	}

	return char_list;
}

/////////////////////////////////////////////////////////////////////////////
///@brief simulate DE-0 Nano
/////////////////////////////////////////////////////////////////////////////
void NanoTask2SimulationTest()
{
		ProtocolWrapper pw;
		protocol_status ps;
		list<string> fields_list;
		list<string> layer1;
		list<string>::iterator it;
		string fld;
		PsychoFrameHeart psycommu;
		char c;

		//simulate clist
		list<char> clist;
		list<string> transmitted_data;
		transmitted_data.push_back("[#[1D1FA7F7#]#[###[cfdata###]###[DirX###]###[up###]#]]");
		transmitted_data.push_back("[#[8B6F67A1#]#[###[cfdata###]###[DirY###]###[up###]#]]");
		transmitted_data.push_back("[#[4EF493B3#]#[###[command###]###[Move###]#]]");
		transmitted_data.push_back("[#[6E289F54#]#[###[cfdata###]###[StepNumX###]###[1###]#]]");
		transmitted_data.push_back("[#[61C2D5C2#]#[###[cfdata###]###[DirX###]###[down###]#]]");

		clist = buildCharList(transmitted_data);

		while(1)
		{
			if(!clist.empty())
			{
						c = clist.front();//read front item
						clist.pop_front();//remove front item
						ps = (protocol_status) pw.input(c);
						//printf("   protocol status = %d\n",ps);
						if(ps == MSG_OK)
						{
							//get fields layer 1
							fields_list = pw.get_fields();
							printf("last message %s \n", pw.last_message.c_str());

							//process fields_list
							try
							{
								if (!(psycommu.input(pw.get_lastmessage()) == 0))
								{
									printf("error non zero exit for psycommu\n");
								}
							}
							catch(...)
							{
								printf("exception processing input\n");
							}
						}

			}
			else
			{
			   //OSTimeDlyHMSM(0, 0, 1, 0);
			    printf("break away\n");
			    break;
			}
		}


}

/////////////////////////////////////////////////////////////////////////////
///@brief verify serial transmission for manual control actions
/////////////////////////////////////////////////////////////////////////////
void maualControlPageTransmitTest()
{
	list<string> transmitted_data;
	transmitted_data.push_back("[#[1D1FA7F7#]#[###[cfdata###]###[DirX###]###[up###]#]]");
	transmitted_data.push_back("[#[8B6F67A1#]#[###[cfdata###]###[DirY###]###[up###]#]]");
	transmitted_data.push_back("[#[4EF493B3#]#[###[command###]###[Move###]#]]");
	transmitted_data.push_back("[#[6E289F54#]#[###[cfdata###]###[StepNumX###]###[1###]#]]");
	transmitted_data.push_back("[#[61C2D5C2#]#[###[cfdata###]###[DirX###]###[down###]#]]");

	processData(buildCharList(transmitted_data));
}


/////////////////////////////////////////////////////////////////////////////
///@brief the new calculated crc value is seeded with the previous crc value
///		  this test verifies in a continuous transmission crc_value is
///		  seeded with previous crc_value
/////////////////////////////////////////////////////////////////////////////
void commandMoveTest()
{
	list<string> transmitted_data;
	transmitted_data.push_back("[#[4EF493B3#]#[###[command###]###[Move###]#]]");
	processData(buildCharList(transmitted_data));
}

/////////////////////////////////////////////////////////////////////////////
///@brief the new calculated crc value is seeded with the previous crc value
///		  this test verifies in a continuous transmission crc_value is
///		  seeded with previous crc_value
/////////////////////////////////////////////////////////////////////////////
void continuousTransmissionTest()
{
	list<string> transmitted_data;
	transmitted_data.push_back("[#[C98074B8#]#[###[cfdata###]###[HMin###]###[2000###]#]]");
	transmitted_data.push_back("[#[958D3E83#]#[###[cfdata###]###[HMax###]###[3000###]#]]");
	transmitted_data.push_back("[#[75755559#]#[###[cfdata###]###[LMin###]###[2100###]#]]");
	transmitted_data.push_back("[#[D8834C4F#]#[###[cfdata###]###[LMax###]###[3100###]#]]");
	transmitted_data.push_back("[#[8A828489#]#[###[cfdata###]###[SPer###]###[50###]#]]");
	transmitted_data.push_back("[#[CE064E32#]#[###[cfdata###]###[HVal###]###[2500###]#]]");
	transmitted_data.push_back("[#[EE52EE94#]#[###[cfdata###]###[LVal###]###[2600###]#]]");
	transmitted_data.push_back("[#[77E292F1#]#[###[cfdata###]###[SInc###]###[10###]#]]");
	transmitted_data.push_back("[#[E19398D0#]#[###[gcode###]###[G90###]#]]");
	transmitted_data.push_back("[#[35E25C71#]#[###[gcode###]###[G21###]#]]");
	transmitted_data.push_back("[#[54B417DF#]#[###[gcode###]###[G0###]###[X6.4844###]###[Y8.4801###]#]]");
	transmitted_data.push_back("[#[5212C9AA#]#[###[gcode###]###[M03###]#]]");
	transmitted_data.push_back("[#[22845C54#]#[###[gcode###]###[G1###]###[F30.000000###]#]]");
	transmitted_data.push_back("[#[B00925A0#]#[###[gcode###]###[G1###]###[X29.4366###]###[Y8.4801###]#]]");
	transmitted_data.push_back("[#[E1EF3E7C#]#[###[gcode###]###[G1###]###[X17.552###]###[Y30.7195###]#]]");
	transmitted_data.push_back("[#[D8B70F45#]#[###[gcode###]###[G1###]###[X6.484###]###[Y8.48###]#]]");
	transmitted_data.push_back("[#[FDC36987#]#[###[gcode###]###[M05###]#]]");
	transmitted_data.push_back("[#[B3D609DF#]#[###[gcode###]###[G0###]###[X0.000###]###[Y0.000###]#]]");
	transmitted_data.push_back("[#[738DB98E#]#[###[gcode###]###[M05###]#]]");
	transmitted_data.push_back("[#[3DAA395E#]#[###[gcode###]###[M02###]#]]");


	processData(buildCharList(transmitted_data));
}
void processCfgDataTest()
{
	list<string> transmitted_data;
	transmitted_data.push_back("[#[C98074B8#]#[###[cfdata###]###[HMin###]###[2000###]#]]");
	transmitted_data.push_back("[#[958D3E83#]#[###[cfdata###]###[HMax###]###[3000###]#]]");
	transmitted_data.push_back("[#[75755559#]#[###[cfdata###]###[LMin###]###[2100###]#]]");
	transmitted_data.push_back("[#[D8834C4F#]#[###[cfdata###]###[LMax###]###[3100###]#]]");
	transmitted_data.push_back("[#[8A828489#]#[###[cfdata###]###[SPer###]###[50###]#]]");
	transmitted_data.push_back("[#[CE064E32#]#[###[cfdata###]###[HVal###]###[2500###]#]]");
	transmitted_data.push_back("[#[EE52EE94#]#[###[cfdata###]###[LVal###]###[2600###]#]]");
	transmitted_data.push_back("[#[77E292F1#]#[###[cfdata###]###[SInc###]###[10###]#]]");

	processData(buildCharList(transmitted_data));
}

void processGCodeTest()
{
	list<string> transmitted_adata;
	list<char> charList;
	transmitted_adata.push_back("[#[8885F03D#]#[###[gcode###]###[G90###]#]]");
	transmitted_adata.push_back("[#[D3BFEF5A#]#[###[gcode###]###[G21###]#]]");
	transmitted_adata.push_back("[#[6BCC3A67#]#[###[gcode###]###[G0###]###[X6.4844###]###[Y8.4801###]#]]");

	charList = buildCharList(transmitted_adata);
	processData(charList);
}

//void printFields(list<string> fields_list)
//{
//	printf("\n");
//	list<string>::iterator it;
//	string fld;
//	int counter = 0;
//	//print fields
//	for (it = fields_list.begin(); it != fields_list.end(); it++)
//	{
//		counter++;
//		fld = (string) *it;
//		printf("field%d = %s\n",counter, fld.c_str());
//	}
//}

void runAllTests(int argc, char const *argv[]){
	cute::suite s;
	//TODO add your test here

	s.push_back(CUTE(processCfgDataTest));
	s.push_back(CUTE(continuousTransmissionTest));
	s.push_back(CUTE(processGCodeTest));
	s.push_back(CUTE(maualControlPageTransmitTest));
	s.push_back(CUTE(NanoTask2SimulationTest));

	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<> >  lis(xmlfile.out);
	cute::makeRunner(lis,argc,argv)(s, "AllTests");
}

int main(int argc, char const *argv[]){
    runAllTests(argc,argv);
    return 0;
}



