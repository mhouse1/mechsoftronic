/////////////////////////////////////////////////////////////////////////////
/// @file   read_and_print_file.cpp
///
/// @date	27 avr. 2015 20:33:38
///
/// @author	Michael House
///
/// @brief	same function used in test_cnc_firmware/test_communication_tcp_based.cpp
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;
void FileReadAndPrint()
{
		//required includes for this to work
//		#include <fstream> //required for ifstream
//		#include <vector>	//required for vector
//		#include <iostream> //required for cout
//
//		using namespace std;

	   ifstream ifs( "c:/bytestream0.txt" );       // note no mode needed

	   //seek get to the end (points to end of file)
	   //so we can call tellg() to get position, which
	   //basically returns the size of file
	   ifs.seekg(0,ifs.end);
	   ifstream::pos_type pos = ifs.tellg();


	   vector<char>  result(pos);
	   if ( ! ifs.is_open() ) {
	      cout <<" Failed to open" << endl;
	   }
	   else {
	      cout <<"Opened OK" <<pos<< endl;
	   }

	   //points to beginning of file again
	   //so when we call read() it reads from beginning
	   ifs.seekg(0,ifs.beg);
	   ifs.read(&result[0], pos);

	   //iterate through vector to print out the whole file
	   for (vector<char>::iterator it = result.begin() ; it != result.end(); ++it)
	     cout <<*it;

	   ifs.close();
}
