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
// Torturer code Generator program
//
// Modification History:
// 01/14/2008: Created by Allen Xu
////////////////////////////////////////////////////////////////////////////

#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "Util/OmnNew.h"
#include "Torturer/Ptrs.h"
#include "Alarm/Alarm.h"
#include "Torturer/FuncTorturer.h"
  
void printHelp()
{
	cout << "Usage: \n"
		<< "TorturerGen.exe \n"
		<< "    -p <productName>:  The product name to which torturer belongs \n"
		<< "    -t <torturerName>: The torturer name\n" 
		<< "    [-f]: 		 forcely overwrite the existing generated code\n" 
		<< "    [-help]:           To print this manual"<< endl;
}


int 
main(int argc, char **argv)
{
	std::string productName;
	std::string torturerName;
	bool force = true;

	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}
	int idx = 0;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printHelp();
			theApp.exitApp();
			return 0;
		}

		if (strcmp(argv[idx], "-p") == 0)
		{
			productName = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-t") == 0)
		{
			torturerName = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-f") == 0)
		{
			idx += 1;
			continue;
		}

		idx++;

	}

	if (productName.empty() || torturerName.empty())
	{
		OmnAlarm << "productName or torturerName can not be empty" << enderr;
		printHelp();
		theApp.exitApp();
		return -1;
	}

	AosFuncTorturer gen(torturerName, productName);
/*	gen.writeToFile();
	theApp.exitApp();
	return 0;
*/
	gen.genCode(force);

	theApp.exitApp();
	return 0;
} 

 
