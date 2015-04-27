#include "cute.h"
#include "test_protocol.h"
#include "protocolwrapper.hpp"
extern "C"
{
#include "crc.h"
}
typedef unsigned long	ULONG;


void Test2_protocolwrapper_wrapfieldscrc() {
	/*
	 * ProtocolWrapper::wrapfieldscrc
	 * will take a list<string> aka fields
	 * then wraps the fields into a single string
	 * with footer header and crc
	 *
	 * for a list<string> value = ['hello','world','i','am','michael']
	 * return a string encoded = [\[14524a2b\]\[hello\]\[world\]\[i\]\[am\]\[michael\]]
	 */
	list<string> myFields;
	myFields.push_back("hello");
	myFields.push_back("world");
	myFields.push_back("i");
	myFields.push_back("am");
	myFields.push_back("michael");

	ASSERT_EQUAL("hello",myFields.front());

	ProtocolWrapper PW;
	string encoded;
	encoded = PW.wrap(myFields);


}

void Test1_protocolwrapper() {
	/*
	 * create an object of ProtocolWraper
	 * create a string message
	 * send one byte at a time into input() of object
	 * verify last_message is decoded message
	 * verify get_fields() returns fields as a list of strings
	 */
	//ASSERTM("start writing tests", false);
	ProtocolWrapper PS;
	string message = "[\\[14524a2b\\]\\[hello\\]\\[world\\]\\[i\\]\\[am\\]\\[michael\\]]";
	//iterate bytes in message
	for (unsigned int i = 0; i<message.length(); i++)
	{
		char c = message[i];
		PS.input(c);
	}
	//cout<<PS.last_message<<endl;
	ASSERT_EQUAL("[14524a2b][hello][world][i][am][michael]",PS.last_message);
	list<string> fields_list;
	fields_list = PS.get_fields();
	//print fields
	list<string>::iterator it;
	for (it =fields_list.begin(); it!=fields_list.end(); ++it)
		cout<<*it<<endl;
		cout<<"\n";
}

void Test1_crc()
{
	ULONG initial_crc_value = 740712820;
	ULONG mycrc = crc32("D",1,initial_crc_value);
	ASSERT_EQUAL(653596365,mycrc);

	cout<<"value of crcval  is "<<(ULONG)mycrc<<endl;
	mycrc = crc32("D",1, mycrc);
	ASSERT_EQUAL(2487406226,mycrc);

	mycrc = crc32("DEAD MOUSE",10, mycrc);
	ASSERT_EQUAL(179102540,mycrc);

	mycrc = crc32("D",1, mycrc);
	ASSERT_EQUAL(248587473,mycrc);
}

void thisIsAtest_protocolTest() {
	ASSERTM("hello", "hello");
}

cute::suite make_suite_test_protocol(){
	cute::suite s;
	s.push_back(CUTE(thisIsAtest_protocolTest));

	//push_back tests to s
	s.push_back(CUTE(Test1_crc));
	s.push_back(CUTE(Test1_protocolwrapper));
	s.push_back(CUTE(Test2_protocolwrapper_wrapfieldscrc));

	return s;
}



