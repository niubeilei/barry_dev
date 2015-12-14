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
#include "StorageEngine/Testers2/FixedLengthTesterMgr.h"

#include "StorageEngine/Testers2/FixedLengthTester.h"
#include "StorageEngine/Testers2/Ptrs.h"
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
vector<int>      			AosFixedLengthTesterMgr::smServer[10];
vector<int>      			AosFixedLengthTesterMgr::smServer1[10];
AosFixedLengthTesterMgr::AosFixedLengthTesterMgr()
:
mTries(eDftTries),
mRecordLength(eDftRecordLength),
mRecordDoc(0),
mNumThreads(1)
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
	init(rdata);

	return true;
}


bool
AosFixedLengthTesterMgr::init(const AosRundataPtr &rdata)
{
	OmnString docstr="<record type=\"fixbin\" ";
	docstr << " zky_name=\"doc\" record_len=\" " << mRecordLength << "\" "
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
		AosXmlTagPtr doc = mRecordDoc->clone(AosMemoryCheckerArgsBegin);
		AosFixedLengthTesterPtr test = OmnNew AosFixedLengthTester(doc, mRecordLength, mRundata[i]);	
		smFixedLength[i] = test;
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
		OmnScreen << "=========== logicid: "<< logicid << endl;
		bool rslt = smFixedLength[logicid]->basicTest(mRundata[logicid]);		

		if (!rslt)
		{
			mCondVar[logicid]->wait(mLock[logicid]);
			continue;
		}
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
AosFixedLengthTesterMgr::updateOutPut(
			const int serverid,  
			const OmnString &key,
			const u32 sizeid, 
			const int docsize,
			const u64 &file_id)
{
	smFixedLength[0]->updateOutPut(serverid, key, sizeid, docsize, file_id);
}


void
AosFixedLengthTesterMgr::docFinished(const int phy, const int data_phy)
{
OmnScreen << "++++++++++: " << phy << " ,  " << data_phy << endl;
 if (smServer[phy].size() == 0) 
 {
	 clearServer();
 }
	smServer[phy][data_phy] = 1;
	for (int i = 0; i<AosGetNumPhysicals(); i++)
	{
		int j = AosGetSelfServerId();
		//for (int j = 0; j<AosGetNumPhysicals(); j++)
		//{
			if (smServer[i][j] == 0) return;
		//}
	}
 OmnScreen << "ccccccccccccccccccccc " << endl;
	smFixedLength[0]->setSaveDocFileMgrFinished();
	clearServer();
}

void
AosFixedLengthTesterMgr::clearServer()
{
	for (int i = 0; i<AosGetNumPhysicals(); i++)
	{
		smServer[i].clear();
		for (int j = 0; j<AosGetNumPhysicals(); j++)
		{
			smServer[i].push_back(0);
		}
	}
}


void
AosFixedLengthTesterMgr::setCreateFileFinished(const int phy, const int data_phy)
{
 OmnScreen << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa " << phy << " , " << data_phy << endl;
 if (smServer1[phy].size() == 0) 
 {
	 clearServer1();
 }
	smServer1[phy][data_phy] = 1;
	for (int i = 0; i<AosGetNumPhysicals(); i++)
	{
		int j = AosGetSelfServerId();
		if (smServer1[i][j] == 0) return;
	}
 OmnScreen << "bbbbbbbbbbbbbbbbbb " << endl;
	smFixedLength[0]->setCreateFileFinished();
	clearServer1();
}

void
AosFixedLengthTesterMgr::clearServer1()
{
	for (int i = 0; i<AosGetNumPhysicals(); i++)
	{
		smServer1[i].clear();
		for (int j = 0; j<AosGetNumPhysicals(); j++)
		{
			smServer1[i].push_back(0);
		}
	}
}


// 1  Actions/ActImportDoc.cpp
//	 a. Note all mTask obj
//	 b. 
//	 	#include "StorageEngine/Testers2/FixedLengthTesterMgr.h"
//		#include "Tester/TestMgr.h"
//	 c. 
//		#include "DocTrans/BatchFixedFinishTrans.h"
//		Note 
//			AosActionObjPtr thisptr(this, false);
//			mTask->actionFinished(thisptr, mRundata);
//			mTask = 0;
//		In the deleteFile() before  
//	 		OmnScreen << "======================== save Finished !!!!!!" << endl;
//	 		AosTransPtr trans = OmnNew AosBatchFixedFinishTrans(mServerid, mDataServerid, false, false, false);
//	 		AosSendTrans(mRundata, trans);
//
//	2.	DataAssembler/DocAssembler.cpp
//		a. appendEntry
//			in the "return info->addDoc(doc, len, docid, rdata);" before
//			OmnString str;
//			str << docid << ":" << "12345abcde"
//			    << ":" << "xyzrsw98765" <<":"
//		       << docid*424 << ":" << docid;
//
//		     const char *doc = str.data();
//		     aos_assert_r(str.length() == 81, false);
//		     len = 81;
//		     AosValueRslt *vv = const_cast<AosValueRslt *>(&value);
//		     vv->setDocid(docid);
//
//		b. Note config()
//			AosXmlTagPtr record = def->getFirstChild(AOSTAG_RECORD);
//			if(!record)
//			{
//			    AosSetErrorU(rdata, "missing record cfg:") << def->toString();
//			    OmnAlarm << rdata->getErrmsg() << enderr;
//			    return false;
//			 }
//
//			 mDataRecord = AosDataRecordObj::createDataRecordStatic(record, mTask, rdata AosMemoryCheckerArgs);
//			 aos_assert_r(mDataRecord, false);
//
//			 mDocSize = mDataRecord->getRecordLen();
//			 aos_assert_r(mDocSize > 0, false);
//
//			 AosXmlTagPtr doc = mDataRecord->getRecordDoc();
//			 aos_assert_r(doc, false);
//
//			 mRecordDoc = doc->clone(AosMemoryCheckerArgsBegin); 
//			modify
//			 mRecordDoc = def->clone(AosMemoryCheckerArgsBegin);
//			 mDocSize = mRecordDoc->getAttrInt("record_len", -1);
//			 mRecordDocid = mRecordDoc->getAttrU64(AOSTAG_DOCID, 0);
//			    
//
//		c. Note all TaskDataObj
//	3. DataAssemble/DocInfo.cpp
//		a. Node all TaskDataObj
//		b. bool rslt = mTask->updateOutPut(task_data, rdata);
//			aos_assert_r(rslt, false);
//			modify
//			#include "StorageEngine/Testers2/FixedLengthTesterMgr.h"
//			#include "Tester/TestMgr.h"
//		c. sendFinished()
//			add 
//			AosTransPtr trans = OmnNew AosBatchFixedFinishTrans(AosGetSelfServerId(), mServerId, true, false, false);
//			AosSendTrans(rdata, trans);
//			AosFixedLengthTesterMgr::updateOutPut(mServerId, mKey, sizeid, mDocSize, file_id);
//
//	4.move BatchFixedFinishTrans to DocTrans/BatchFixedFinishTrans.cpp
//	modify DocTrans/AllDocTrans.cpp
//		
