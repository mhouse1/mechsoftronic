/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @date	Sep 8, 2014 3:22:49 PM
///
/// @author	Michael House
///
/// @brief	[see corresponding .hpp file]
///
/// @par Copyright (c) 2014 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <cstring>
#include <sstream>
#include "protocolwrapper.hpp"

extern "C"
{
#include "crc.h"
}

ProtocolWrapper::ProtocolWrapper()
{
    this->decode_state = WAIT_HEADER;
    this->pstatus = START_MSG;
    this->Header = 91;
    this->Dle = 35;
    this->Footer = 93;
    this->message_buf = "";
    this->last_message = "";
    this->keep_header = false;
    this->keep_footer = false;
    this->crc_value = 740712820;
}

string ProtocolWrapper::wrap(list<string> fields)
{
	/*
	 * ProtocolWrapper::wrap
	 * will take a list<string> aka fields
	 * then wraps the fields into a single string
	 * with footer header and crc
	 *
	 * for a list<string> value = ['hello','world','i','am','michael']
	 * return a string value_string = [\[14524a2b\]\[hello\]\[world\]\[i\]\[am\]\[michael\]]
	 */
	list<string>::iterator field;
	string wrapped_fields = "";
	string fld;
	ULONG last_crc_value;

	unsigned int i;
	for (field =fields.begin(); field!=fields.end(); ++field)
	{
		wrapped_fields += (char) this->Header;
		//cout<<*field<<endl;

		//get string from iterator
		fld = (string)*field;

	    for(i = 0; i<fld.length(); i++)
	    {
	    	char c = fld[i];
	    	if ( (c == this->Header) || (c == this->Footer) || (c == this->Dle))
	    	{
	    		wrapped_fields += this->Dle + c;
	    	}
	    	else
	    	{
	    		wrapped_fields += c;
	    	}
	    }
	    //complete field by adding footer
	    //at the end of for loop it will be a string
	    //of all fields with header and footer for each field
	    //e.g. [hello][world][i][am][michael]
	    wrapped_fields += this->Footer;

	}
	//cout <<wrapped_fields<<endl;
	last_crc_value = this->crc_value;

	this->crc_value = crc32((char *) wrapped_fields.c_str(),wrapped_fields.length(),last_crc_value);
	//cout << this->crc_value <<endl;

	//Converts ulVal into hex string crc_string
	ULONG ulVal, ulShift, ulDigit;
	signed short m;
	ulVal = this->crc_value;
	ulShift = 28;
	string crc_string = "";
	for (m = 8; m > 0; m--)
	{
	    ulDigit = (ulVal >> ulShift) & 0x0000000F;	// Get nibble of interest
	    if (ulDigit <= 9)
		  //cout<<(char)(ulDigit + '0');
	    	crc_string += (char)(ulDigit + '0');
	    else
		  //cout<<(char)(ulDigit - 10 + 'A');
	    	crc_string +=(char)(ulDigit - 10 + 'A');
	    ulShift -=4;
	}

	//frame the crc_string and append wrapped_fields
	wrapped_fields = (char)this->Header+crc_string+(char)this->Footer + wrapped_fields;

	string wrap_again = "";
	wrap_again += (char) this->Header;
	//get string from iterator
	fld = wrapped_fields;

    for(i = 0; i<fld.length(); i++)
    {
    	char c = fld[i];
    	//cout<<c<<endl;
    	//wrap_again += c;
    	if ( (c == this->Header) || (c == this->Footer) || (c == this->Dle))
    	{
    		wrap_again.append("#");// += (char)this->Dle + c;
    		wrap_again += c;
    	}
    	else
    	{
    		wrap_again += c;
    	}
    }
    //complete field by adding footer
    wrap_again += this->Footer;

	return wrap_again;
}
/*!
 * decodes this->last_mesage [14524a2b][hello][world][i][am][michael]
 * returns a list<string>  14524a2b, hello, world, i, am, michael
 */
string ProtocolWrapper::get_lastmessage(void)
{
	return this->last_message;
}
/*!
 * decodes this->last_mesage [14524a2b][hello][world][i][am][michael]
 * returns a list<string>  14524a2b, hello, world, i, am, michael
 */
list<string> ProtocolWrapper::get_fields(void)
{
	ProtocolWrapper pw;
	string raw_fields = this->last_message;
    list<string> field_list;
    //c++ 11 style
//    for(char& byte: raw_fields)
//    {
//    	if(pw.input(byte) == MSG_OK)
//    	{
//    		field_list.push_back(pw.last_message);
//    	}
//    }
    for(unsigned int i = 0; i<raw_fields.length(); i++)
    {
    	if(pw.input(raw_fields[i]) == MSG_OK)
    	{
    		field_list.push_back(pw.last_message);
    	}
    }
    return field_list;
}
/*!
 * decodes a string
 * returns and returns a list of fields
 */
list<string> ProtocolWrapper::get_fields(string message)
{
	ProtocolWrapper pw;
	string raw_fields = message;
    list<string> field_list;
    for(unsigned int i = 0; i<raw_fields.length(); i++)
    {
    	if(pw.input(raw_fields[i]) == MSG_OK)
    	{
    		field_list.push_back(pw.last_message);
    	}
    }
    return field_list;
}

int ProtocolWrapper::finish_msg(void)
{
	this->decode_state = WAIT_HEADER;
	this->last_message = this->message_buf;
	this->message_buf = "";
	return this->pstatus = MSG_OK;
}

int ProtocolWrapper::input(UBYTE new_byte)
{
	switch(decode_state)
	{
	case(WAIT_HEADER):
		if(new_byte == this->Header)
		{
			if (this->keep_header)
				this->message_buf += (char)new_byte;
			this->decode_state = IN_MSG;
			return this->pstatus = START_MSG;
		}
		else
		{
			cout <<"expected header "<< (char) this->Header <<", got "<<(char) new_byte<< endl;
			return this->pstatus = ERROR;
		}
	break;
	case(IN_MSG):
		if(new_byte == this->Dle)
		{
			this->decode_state = AFTER_DLE;
			return this->pstatus = BUILDING_MSG;
		}
		else if (new_byte == this->Footer)
		{
			if (this->keep_footer)
				this->message_buf += (char) new_byte;
			return finish_msg();
		}
		else
		{
			this->message_buf += new_byte;
			return this->pstatus = BUILDING_MSG;
		}
	break;
	case(AFTER_DLE):
		this->message_buf += new_byte;
		this->decode_state = IN_MSG;
		return this->pstatus = BUILDING_MSG;
	break;
	default:
		return -1;
	break;
	}

}


