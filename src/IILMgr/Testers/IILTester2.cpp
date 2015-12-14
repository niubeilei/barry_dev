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
// 08/09/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/Testers/IILTester2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/Ptrs.h"
#include "IILMgr/IILStr.h"
#include "IILTrans/AllIILTrans.h"
#include "SEUtil/IILIdx.h"
#include "Random/RandomBuffArray.h"
#include "Thread/ThreadPool.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "Util/File.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"


AosIILTester2::AosIILTester2()
{
}

AosIILTester2::~AosIILTester2()
{
}

bool 
AosIILTester2::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);

	OmnThreadPool threadPool("AosIILTester2", __FILE__, __LINE__); 

	vector<OmnThrdShellProcPtr> v;
	for (int i=0; i<8; i++)
	{
		OmnThrdShellProcPtr t = OmnNew RunTest(this, mRundata.getPtr(), i);	
		v.push_back(t);
	}

	threadPool.procSync(v); 

	return true;
}


bool 
AosIILTester2::testStart(const int id, const AosRundata* rdata)
{
	OmnString fname = "data/x0";
	fname << id;
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file.isGood(), false);
	u64 filelength = file.getLength();
	AosBuffPtr buff = OmnNew AosBuff(filelength AosMemoryCheckerArgs);
	file.readToBuff(0, filelength, buff->data());

	buff->setDataLen(filelength);
	char* begin = buff->data();
	char* end = begin;
	int idx = 0;

	vector<AosIILTransPtr> transv;
	while(end < buff->data()+filelength)
	{
		if (*end ==	'\n')
		{
			OmnString s(begin, end-begin);
			AosStrSplit sp(s, ",");
			vector<OmnString> v = sp.entriesV();
			AosIILTransPtr trans;
			switch(v[0][0])
			{
			case '1':
				 if (v.size() == 3)
					 trans = OmnNew AosIILTransHitAddDocByName(v[1], false, atoi(v[2].data()), false, false);
				 break;
			case '2':
				 if (v.size() == 4 && v[2].length()<400)
					 //trans = OmnNew AosIILTransStrAddValueDocByName(v[1], false, v[2], atoi(v[3].data()), false, true, false, false);
				 break;
			case '3':
				 if (v.size() == 4)
					 //trans = OmnNew AosIILTransU64AddValueDocByName(v[1], false, atoi(v[2].data()), atoi(v[3].data()), false, false, false, false);
				 break;
			}

			if (trans)
			{
				u64 iilid = trans->getIILID(mRundata.getPtr());
				trans->setRundata(mRundata);
				AosIILMgr::getSelf()->addTrans(iilid, 100, trans, mRundata);
			}
			else
			{
				//OmnScreen << s << endl;
			}
			begin = end+1;
		}

		end++;
	}
	
	return true;
}
