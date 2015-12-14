#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "util/tracer.h"
#include "util/global_data.h"


int main(int argc, char* argv[])
{
	aos_global_data_init();
  	aos_tracer_set_filter(eAosMD_Tracer, eAosLogLevel_Debug);

	// Get the top level suite from the registry
	CppUnit::Test *suite = 
		CppUnit::TestFactoryRegistry::getRegistry().makeTest();
	  
	// Adds the test to the list of test to run
	CppUnit::TextUi::TestRunner runner;
	runner.addTest( suite );
	  
	// Change the default outputter to a compiler error 
	// format outputter
	runner.setOutputter(new CppUnit::CompilerOutputter( 
				  &runner.result(), std::cerr ) );

	// Run the tests.
	bool wasSucessful = runner.run();
	  
	// Return error code 1 if the one of test failed.
	return wasSucessful ? 0 : 1;
}
