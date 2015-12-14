////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/ThreadDef.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include "AppProc/http_proc/Tester/HttpTestSuite.h"
#include <stdio.h>

#define  MAX_BUFF_SIZE 500000
char *HTTP_DATA;
unsigned HTTP_DATA_SIZE;
unsigned int THREAD_COUNT=1;
unsigned int THREAD_NUM=1;
char* FILE_NAME = "./Data_Get";

void cliInterface();

unsigned int cmd_get_thread_num(int argc, char **argv)
{
	unsigned int ret = 1;	//default thread number is 1
	for (int i = 0; i< argc; i++)
	{
		if (argc ==1)
			return ret;
		unsigned int num = 0;
		if (sscanf(*(argv+i), "thread=%u", &num)!=0)
		{
			ret = num;
			break;
		}
		
	}
	return ret;
	
}

unsigned int cmd_get_thread_count(int argc, char **argv)
{	
	unsigned int ret = 1;
	for (int i = 0; i< argc; i++)
	{
		if (argc == 1)
			return ret;
		unsigned int num = 0;
		if (sscanf(*(argv+i), "count=%u", &num)!=0)
		{
			ret = num;
			break;
		}
	}
	return ret;
}

char* cmd_get_file_name(int argc, char **argv)
{
	char* ret = FILE_NAME;
	for (int i = 0; i< argc; i++)
	{
		if (argc ==1)
			return ret;
		if (memcmp(*(argv+i), "file=", 5) == 0)
		{
			return *(argv+i) + 5;
			break;
		}
	}
	return FILE_NAME;
}

int 
main(int argc, char **argv)
{

	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
		theApp.startSingleton(OmnNew OmnCliProcSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " 
			<< e.toString() << endl;
		return 0;
	}
	if (argc < 1)
	{
		cout << "Usage: HttpTest.exe filename\n";
		return 0;
	}
	//Read http samples data from file
	char* file_name = cmd_get_file_name(argc, argv);
	cout << "Read data from file: "<< file_name << endl;
	FILE *File;
	File = fopen(file_name, "r");
	if (!File)
	{
		cout << "Open HttpData file error!" << endl;
		exit(1);
	}
	fseek(File, 0, SEEK_END);
	unsigned long file_len = ftell(File);
	fseek(File, 0, SEEK_SET);
	HTTP_DATA_SIZE = file_len;
	HTTP_DATA = (char*)malloc(file_len+1);

	if (!File)
	{
		cout << "Open File failed\n";
		return 0;
	}
	
	int nRead;
	memset(HTTP_DATA, 0x00, file_len+1);
	nRead = fread((char*)HTTP_DATA, 1, file_len, File);
	if (nRead == 0)
	{
		cout << "Import File Error\n";
		return 0;
	}

	THREAD_NUM = cmd_get_thread_num(argc, argv);
	cout << "Thread = " << THREAD_NUM << endl;
	
	THREAD_COUNT = cmd_get_thread_count(argc, argv);
	cout << "Count = " << THREAD_COUNT << endl;


    //OmnKernelApi::setKernelApiFunc(AosKernelApi_setSockopt);
	//OmnKernelApi::init();


   // cliInterface();

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("HttpProc Tester", "Try", "Li Qi");
    testMgr->addSuite(HttpTestSuite::getSuite());

    cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

    //char c = 0;
    //while (c != '0')
    //{
        //cliInterface();
        //continue;
    //}

	theApp.appLoop();
	return 0;
} 

/*
void cliInterface()
{
    const int cmdlen = 1000;
    char cmd[cmdlen];
    OmnString rslt;
    bool skip = false;

    while (1)
    {
        if (!skip)
        {
            cout << "(PacketEngine)# " << flush;
        }

        int index = 0;
        while (index < cmdlen)
        {
            // char c = getchar();
            char c;
            read(1, &c, 1);
            if (c == '\n' || c == EOF)
            {
                cmd[index++] = 0;
                break;
            }
            cmd[index++] = c;
        }

        rslt = "";

        if (strlen(cmd) > 3)
        {
            skip = false;
			if (strcmp(cmd, "exit") == 0 )
				break;
            OmnCliProc::getSelf()->runCli(cmd, rslt);
            cout << rslt << endl;
        }
        else
        {
            skip = true;
        }
    }
}


int AosCliSimu_kernelInit(void)
{
    OmnKernelApi_init();
    AosCertMgr_init();
    AosCertChain_init();
    return 0;
}
*/
