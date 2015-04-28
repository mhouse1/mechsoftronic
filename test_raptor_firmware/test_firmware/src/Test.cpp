#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

//import test suites
#include "test_protocol.h"
#include "test_cncmachine.h"
#include "test_communication_tcp_based.h"


void runSuite_protocol(int argc, char const *argv[]){
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);
	cute::suite s=make_suite_test_protocol();
	cute::makeRunner(lis,argc,argv)(s, "test_protocol");
}



void runSuite_cncmachine(int argc, char const *argv[]){
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);
	cute::suite s=make_suite_test_cncmachine();
	cute::makeRunner(lis,argc,argv)(s, "test_cncmachine");
}

void runSuite_comm_tcp_based(int argc, char const *argv[]){
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);
	cute::suite s=make_suite_test_comm_tcp_based();
	cute::makeRunner(lis,argc,argv)(s, "test_comm_tcp_based");
}


int main(int argc, char const *argv[]){
	runSuite_protocol(argc,argv);
    runSuite_cncmachine(argc,argv);
    runSuite_comm_tcp_based(argc,argv);
}



