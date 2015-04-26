/////////////////////////////////////////////////////////////////////////////
/// @file   psychoframe.cpp
///
/// @date	21 sept. 2014 10:37:45
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include "psychoframe.hpp"

PsychoFrame::PsychoFrame()
{
	//multi_list;

}


/////////////////////////////////////////////////////////////////////////////
///@brief takes raw string data as input and returns list of list<string>
/// 	  called multi_list. The design is such that multi_list can hold
///		  any number of layers in an encodeddata message
/////////////////////////////////////////////////////////////////////////////
list<list<string> > PsychoFrame::data_in(string data)
{
	ProtocolWrapper pw;
	list<string> layer;
	list<string>::iterator it;
	layer = pw.get_fields(data);
	this->multi_list.push_back(layer);
	it = layer.begin();
	it++;
	data = (string) *it;
	layer = pw.get_fields(data);
	this->multi_list.push_back(layer);
//@todo implement so multi_list will be dynamic able to hold any number of layers

	return this->multi_list;
}
