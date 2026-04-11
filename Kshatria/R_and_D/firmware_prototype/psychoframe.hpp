/////////////////////////////////////////////////////////////////////////////
/// @file   psychoframe.hpp
///
/// @date	21 sept. 2014 10:37:27
///
/// @author	Michael House
///
/// @brief
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef PSYCHOFRAME_HPP_
#define PSYCHOFRAME_HPP_
#include <list>
#include "protocolwrapper.hpp"


using namespace std;
class PsychoFrame
{
public:
	PsychoFrame();
	//list< list<string> > multi_list;
	list<list<string> > multi_list;
	std::list<std::string> layer1_fields;
	std::list<std::string> layer2_fields;

	//multi_list.push_back(layer1_fields);

	list<list<string> > data_in(string data);


	//access PsychoFrames layer1 field_list and process it.
	//where the field_list is a list<string> of fields.
	//virtual void processLayer1() = 0;

};



#endif /* PSYCHOFRAME_HPP_ */
