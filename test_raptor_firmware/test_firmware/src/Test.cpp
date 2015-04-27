#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"
#include "test_protocol.h"


void runSuite(int argc, char const *argv[]){
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);
	cute::suite s=make_suite_test_protocol();
	cute::makeRunner(lis,argc,argv)(s, "test_protocol");
}

int main(int argc, char const *argv[]){
    runSuite(argc,argv);
}



