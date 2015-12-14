#include "Porting/Mutex.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "DataCalc/DataCalc.h"
#include "Util/DLLObj1.h"
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#define MAX_STRING      80

// bool OmnAppExecutionIsActive = true;


void invoke_method(char *lib, char *method, const AosRundataPtr &argument)
{
	/* Open the shared object */
	void *dl_handle = dlopen(lib, RTLD_LAZY);
	if (!dl_handle)
	{
		printf( "!!! %s\n", dlerror());
		return;
	}

	/* Resolve the symbol (method) from the object */
	bool (*func)(const AosRundataPtr &rdata);
	func = (bool(*)(const AosRundataPtr &rdata))(dlsym(dl_handle, method));
	char *error = dlerror();
	if (error != NULL)
	{
		printf("!!! %s\n", error);
		return;
	}
	/* Call the resolved method and print the result */
	// printf("  %f\n", (*func)(argument));
	printf("Calling the function\n");
	(*func)(argument);

	/* Close the object */
	dlclose(dl_handle);
	return;
}

int main(int argc, char *argv[])
{
	char line[MAX_STRING+1];
	char lib[MAX_STRING+1];
	char method[MAX_STRING+1];
	AosRundataPtr argument = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	argument->setSiteid(100);

	AosDataCalc datacalc("", "", "", "");
	AosDLLObj1 obj(AosDLLObj1::eDataCalc, "", "", "");
	while(1)
	{
		printf("> ");
		line[0]=0;
		fgets( line, MAX_STRING, stdin);
		if (!strncmp(line, "bye", 3)) break;
		printf("%s", line);
		sscanf(line, "%s %s", lib, method);
		printf("Before invoking\n");
		invoke_method(lib, method, argument);
		printf("after invoking\n");
	}
}

int test(float argument)
{
	return 1;
}

//	mtj@camus:~/dl$ gcc -rdynamic -o dl dl.c -ldl
//	mtj@camus:~/dl$ ./dl
//	> libm.so cosf 0.0
//	  1.000000
//  > libm.so sinf 0.0
//    0.000000
//  > libm.so tanf 1.0
//    1.557408
//  > bye
//	mtj@camus:~/dl$
//
// gcc -rdynamic -o DlApi DlApi.cpp -ldl -lstdc++
// gdb 
// gcc -rdynamic -o DlApi Chen.cpp -I../.. -DOMN_PLATFORM_UNIX -ldl -lstdc++ -g
// gcc -fpic -shared -Wall -I../.. -DOMN_PLATFORM_UNIX Chen.cpp -o Chen 
// gcc -rdynamic -o Chen Chen.cpp -I../.. -DOMN_PLATFORM_UNIX -ldl -lstdc++
//
// libTest.so ChenDingTest
// http://www.ibm.com/developerworks/cn/linux/l-dynamic-libraries/
