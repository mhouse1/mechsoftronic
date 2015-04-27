#include "cute.h"
#include "test_protocol.h"

void thisIsAtest_protocolTest() {
	ASSERTM("start writing tests", false);	
}

cute::suite make_suite_test_protocol(){
	cute::suite s;
	s.push_back(CUTE(thisIsAtest_protocolTest));
	return s;
}



