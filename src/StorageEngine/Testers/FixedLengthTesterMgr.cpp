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
// 2013/03/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/Testers/FixedLengthTesterMgr.h"

#include "StorageEngine/Testers/FixedLengthTester.h"
#include "StorageEngine/Testers/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "XmlUtil/SeXmlParser.h"
#include "StorageEngine/Ptrs.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "DocClient/DocClient.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Random/RandomUtil.h"
#include "Util/StrSplit.h"


AosFixedLengthTesterPtr      AosFixedLengthTesterMgr::smFixedLength[eMaxThrds];
OmnMutexPtr					AosFixedLengthTesterMgr::smTransLock = OmnNew OmnMutex(); 

AosFixedLengthTesterMgr::AosFixedLengthTesterMgr()
:
mTries(eDftTries),
mRecordLength(eDftRecordLength),
mRecordDoc(0),
mNumThreads(4)
{

}

AosFixedLengthTesterMgr::~AosFixedLengthTesterMgr()
{
}



bool 
AosFixedLengthTesterMgr::start()
{
	OmnScreen << "    Start Tester ..." << endl;
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	rdata->setUserid(307);

	config();
	createNormalDoc(rdata);

	init(rdata);
	return true;
}


bool
AosFixedLengthTesterMgr::init(const AosRundataPtr &rdata)
{
	OmnString docstr="<record type=\"fixbin\" ";
	docstr << " zky_name=\"doc\" record_len=\" " << mRecordLength << "\" "
		//<< " zky_objid=\"unicom_ac_config_" << mRecordLength << "\" "
		//<< " zky_pctrs=\"sdoc_unicom_doc\" zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
		<< " zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
		<< " <element type=\"str\" zky_name=\"str\" zky_rawformat=\"str\" "
		<< " zky_length=\"" << mRecordLength << "\" "
		<< " zky_offset=\"0\"/>"
		<< " </record> ";

	AosXmlParser parser;
	mRecordDoc = parser.parse(docstr, "" AosMemoryCheckerArgs);

	if(!mThread[0])
	{
		startThread(rdata);
	}
	return true;
}

bool
AosFixedLengthTesterMgr::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("fixed_length_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
}


void
AosFixedLengthTesterMgr::startThread(const AosRundataPtr &rdata)
{
	OmnThreadedObjPtr thisptr(this, false); 
	for (u32 i = 0; i< mNumThreads; i++)
	{
		mThread[i] = OmnNew OmnThread(thisptr, "FixedLengthTesterMgr", i, false, true, __FILE__, __LINE__);
		mLock[i] = OmnNew OmnMutex();
		mCondVar[i] = OmnNew OmnCondVar();
		mRundata[i] = rdata->clone(AosMemoryCheckerArgsBegin);
		mJobId[i] = i * 100000;
		AosXmlTagPtr doc = mRecordDoc->clone(AosMemoryCheckerArgsBegin);
		AosFixedLengthTesterPtr test = OmnNew AosFixedLengthTester(doc, mRecordLength, i*100000, mRundata[i]);	
		smFixedLength[i] = test;
		mVary[i] = 0;
		mThread[i]->start(); 
	}
}

bool
AosFixedLengthTesterMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{

	while (state == OmnThrdStatus::eActive)
	{
		u32 logicid = thread->getLogicId();
		mLock[logicid]->lock();
		OmnScreen << "=========== logicid: "<< logicid << " , tries: "<< mVary[logicid] << endl;
		if (mVary[logicid] >= mTries)
		{
			smFixedLength[logicid]->finish(mRundata[logicid]);		
OmnScreen << "==================== logicid: " << logicid << "finish!!"<< endl;
			mCondVar[logicid]->wait(mLock[logicid]);
			continue;
		}
		if (mVary[logicid] == 20000)
		{
			smFixedLength[logicid]->setNumChecksPerTry(2000);
			smFixedLength[logicid]->setWeightCheckContents(30);
		}
		smFixedLength[logicid]->basicTest(mRundata[logicid]);		
		mVary[logicid] ++;
		mLock[logicid]->unlock();
	}
	return true;
}

bool 
AosFixedLengthTesterMgr::signal(const int threadLogicId)
{
	return true;
}

bool
AosFixedLengthTesterMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

void
AosFixedLengthTesterMgr::docFinished(const u64 &jobid, const u32 &serverid)
{
	//smTransLock->lock();
	int index = jobid/100000;	
	aos_assert(index <eMaxThrds);
OmnScreen << "index: " << index << " , jobid: " << jobid << endl;
	aos_assert(smFixedLength[index]);	
	smFixedLength[index]->updateServer(serverid);

	if (smFixedLength[index]->checkServerFinished())
	{
		smFixedLength[index]->docFinished();
	}
	//smTransLock->unlock();
}

bool
AosFixedLengthTesterMgr::createNormalDoc(const AosRundataPtr &rdata)
{
	OmnString docids;
	for (int i = 0; i<550; i++)
	{
		OmnString docstr;
		docstr << "<vpd zky_a = \"11\"  zky_otype=\"doc\" zky_pctrs=\"vpd\" zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
			 << "<str>ksjdflskjdflksjdlfkjsdl</str>"	
			 << "</vpd>";
		AosXmlTagPtr dd = AosCreateDoc(docstr, true, rdata);
		aos_assert_r(dd, false);
		u64 docid = dd->getAttrU64(AOSTAG_DOCID, 0);
		docids << ", " << docid;
	}
OmnScreen << "docids :" << docids << endl; 
}

// 1.Note Job/JobMgr    cleanFixedDocFinished() 
// int self_serverid = AosGetSelfServerId();                
// if (self_serverid == job_serverid)
// {
//     return cleanFixedDocFinishedLocal(
//     job_docid, logic_taskid, server_id, msg, rdata);
//  }
//
//  2.modify JobTrans/CleanFixedDocFinishedTrans.cpp
//  proc() 
//  bool rslt = AosJobMgr::getSelf()->cleanFixedDocFinishedLocal(
//  mJobDocid, mLogicTaskId, mServerId, mMsg, mRdata);
//  if (!rslt)
//  {
//     mRdata->setError();
//     return true;
//  }
//
//	#include "StorageEngine/Testers/FixedLengthTesterMgr.h"
//	#include "Tester/TestMgr.h"
//
//	AosFixedLengthTesterMgr::docFinished(mJobDocid, mServerId);
//
//	3. StoageEngine/StorageEngineMgr.cpp
//	Note jobmgr->updateFixedDocProgress();
