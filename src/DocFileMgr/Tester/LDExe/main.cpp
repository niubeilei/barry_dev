////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "Porting/Sleep.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void aosSignalHandler(int value)
{
	if (value == SIGALRM)
	{
		int i = 0;	
	}
}


int
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	
	while(1)
	{
		int pid = fork();
		cout << "child pid:" << pid << endl;

		if(pid == 0)
		{
			char *argv2 = "tttt";
			execlp("../DfmTester.exe", argv2);
			return 0;
		}
		else if(pid > 0)
		{
			int pr = wait(NULL);	
			cout << "svr deathed:" << pr << endl;
		}
		OmnSleep(10);
	}
	
	return 1;
}


