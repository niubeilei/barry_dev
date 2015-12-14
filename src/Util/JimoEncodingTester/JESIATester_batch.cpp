#include <stdio.h>
#include "AppMgr/App.h"
#include "Util/JimoEncodingTester/JESIATorturer_batch.h"
#include "Util/OmnNew.h"


int 
main(int argc, char **argv)
{
	int index = 1;
	int tries = 100 * 1000 * 1000;
	while (index < argc)
	{
		if (strcmp(argv[index], "-tries") == 0)
		{
            tries = atoi(argv[index+1]);
			break;
		}
		index++;
	};
	OmnApp theApp(argc, argv);
	srand(time(NULL));
	AosJESIATorturer_batch *tt = OmnNew AosJESIATorturer_batch();
    tt->start(tries);

	return 0;

}
