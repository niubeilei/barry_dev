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
// Modification History:
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
/*#include "IILMerger/Tester/IILMergerTester2.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "IILMgr/Ptrs.h"
#include "IILMerger/Ptrs.h"
#include "IILMerger/IILMerger.h"
#include "IILMerger/IILMergerBkt.h"
#include "IILMerger/IILMergerHandlerStrAddAttrBatch.h"
#include "IILClient/IILTransAsmStrInc.h"
#include "IILClient/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Porting/Sleep.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/BuffArray.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
//#include <stdlib.h>
//#include <set>
//using namespace std;

multiset<string>  AosIILMergerTester2::smCheck000;

AosIILMergerTester2::AosIILMergerTester2()
{
}


AosIILMergerTester2::~AosIILMergerTester2()
{
}


void procset1(string &name)
{
	multiset<string>::iterator itr;
	if ((itr = AosIILMergerTester2::smCheck000.find(name)) == AosIILMergerTester2::smCheck000.end())
	{
		OmnAlarm << "proc error!" << enderr;
		return ;
	}
	AosIILMergerTester2::smCheck000.erase(itr);
}

static char sg[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
             'h', 'i', 'j', 'k', 'l', 'm', 'n',
             'o', 'p', 'q', 'r', 's', 't', 'u',
             'v', 'w', 'x', 'y', 'z'};

static void randstr(char *buff, int maxlen)
{
	int len = rand()%maxlen +1;
	for (int i=0; i<len; i++)
	{
		if (len%2)
		{
			buff[i] = sg[rand()%26];
		}
		else
		{
			buff[i] = sg[rand()%13];
		}
	}
	buff[len] = 0;
}


#include <signal.h>
void finallycheck(int value)             
{
	if (value == SIGALRM) 
		OmnScreen << "SIGALRM" << endl;
}

bool 
AosIILMergerTester2::start()
{
	signal(SIGALRM, finallycheck);
	OmnScreen << "    Start Tester ..." << endl;
	{
		AosCompareFunPtr cmp = OmnNew AosFunStrU642(40, true);

		AosXmlTagPtr asm1 = OmnApp::getAppConfig()->getFirstChild("asm");
		aos_assert_r(asm1, false);
		AosIILTransAssemblerPtr assembler = 
			OmnNew AosIILTransAsmStrInc(NULL, asm1, OmnApp::getRundata());

		AosRundataPtr rdata = OmnApp::getRundata();
		rdata->setSiteid(100);
		assembler->sendStart(rdata);
		for (u32 i=0; i<1000000000; i++)
		{
			char buff[128];
			randstr(buff, 31);
			smCheck000.insert(buff);
			assembler->appendEntry(buff, strlen(buff), i, rdata);
		}
		assembler->sendFinish(rdata);
		//alarm(10000);
		OmnScreen << "Sleeping..." << endl;
		sleep(100000);
		OmnScreen << "Checking..." << endl;
		if (!smCheck000.empty())
		{
			OmnAlarm << enderr;
		}
	}
	return true;
}
*/
