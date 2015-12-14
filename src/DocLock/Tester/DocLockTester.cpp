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
// 06/15/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocLock/Testers/DocLockTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "DocLock/DocLock.h"

AosDocLockTester * docLockTester = NULL;
OmnThreadPtr smThread = 0;

AosDocLockTester::AosDocLockTester()
{
	docLockTester = this;
	mLock = OmnNew OmnMutex();
}


bool 
AosDocLockTester::start()
{
	cout << "    Start AosDocLock Tester ..." << endl;
	basicTest();
	return true;
}

bool
AosDocLockTester::basicTest()
{
	if (!smThread)	smThread = OmnThreadMgr::getSelf()->getCurrentThread();
	if (smThread->getThreadId() != (OmnThreadMgr::getSelf()->getCurrentThread())->getThreadId())
	{
		OmnAlarm << enderr;
	}
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		//cout << "i:"<< i << endl;
		aos_assert_r(testOneDocLock(), false);
		//OmnSleep(1);
	}
	return true;
}

bool
AosDocLockTester::testOneDocLock()
{
	AosRundataPtr rdata = OmnApp::getRundata(); 
	rdata->setSiteid("100");
	int vv = rand() %100;
	if (vv < 60)
	{//read lock
		aos_assert_r(testReadLock(rdata), false);
		return true;
	}
	aos_assert_r(testWriteLock(rdata), false);
	return true;
}


bool
AosDocLockTester::testReadLock(const AosRundataPtr &rdata)
{

	int vv = rand()%100;
	if (vv <60)
	{
		aos_assert_r(ReadLock(rdata), false);
	}
	else
	{
		aos_assert_r(ReadUnLock(rdata), false);
	}
	return true;
}

bool
AosDocLockTester::testWriteLock(const AosRundataPtr &rdata)
{
	int vv = rand()%100;
	if (vv <60)
	{
		aos_assert_r(WriteLock(rdata), false);
	}
	else
	{
		aos_assert_r(WriteUnLock(rdata), false);
	}
	return true;
}

bool
AosDocLockTester::ReadLock(const AosRundataPtr &rdata)
{
	u64 docid = pickDocid();
	u64 userid = pickUserid();
	rdata->setUserid(userid);
	bool results = false;
	//check docid, userid
 	TestReadItr_t iter = mReadMap.find(docid);	
	TestWriteItr_t itw = mWriteMap.find(docid);
	if (iter == mReadMap.end() && itw == mWriteMap.end())
	{
		results = true;
	}
	if (!results)
	{
		if (iter != mReadMap.end() && itw == mWriteMap.end())
		{
			TestMap uu1 = iter->second;
			if (uu1.count(userid) == 0)
			{
				results = true;
			}
		}
	}
	//cout << "ReadLock" <<endl;
	printRead(rdata);
	//cout << "**********"<<endl;
	u32 lock_timer = 1;//OmnGetSecond();
	bool rslt =	AosDocLock::getSelf()->readLock(rdata, docid, lock_timer);
	aos_assert_r(rslt, false);
	OmnString contents = rdata->getContents();
	if (!results)
	{
		aos_assert_r(rdata->getErrcode() == eAosXmlInt_General, false); 
		return true;
	}
	aos_assert_r(rdata->getErrcode() == eAosXmlInt_Ok, false);
	AosXmlParser parser;
	AosXmlTagPtr contentsxml = parser.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_r(contentsxml, false);
	contentsxml = contentsxml->getFirstChild();
	aos_assert_r(contentsxml, false);
	contentsxml = contentsxml->getFirstChild();
	aos_assert_r(contentsxml, false);
	u64 lockid = contentsxml->getAttrU64(AOSTAG_LOCK_DID, 0);
	aos_assert_r(lockid, false);

	if (mReadMap.count(docid)== 0)
	{
		//aos_assert_r(mReadMap[docid].empty() == true, false);
		mReadMap[docid][userid] = lockid;
		//map<u64, u64> mm;
		//mm.insert(make_pair(userid, lockid));
		//mReadMap.insert(make_pair(docid, mm)); 	
		aos_assert_r(mReadMap[docid][userid] == lockid, false);
	}
	else
	{
		aos_assert_r(mReadMap[docid].count(userid)==0, false) ;
		//mReadMap[docid].insert(userid, lockid);
		//mReadMap[docid][userid] = lockid;
		//TestMap mm;
		//mm.insert(make_pair(userid, lockid));
		//mReadMap[docid] = mm;
		mReadMap[docid][userid] = lockid;
		aos_assert_r(mReadMap[docid][userid] == lockid, false);
	}

	aos_assert_r(mReadMap[docid][userid] == lockid, false);
//AosDocLock::getSelf()->timeout(0, rdata);
	printRead(rdata);
	//cout <<"ReadLock" << endl;
//timeout(rdata);
	return true;
}

bool
AosDocLockTester::ReadUnLock(const AosRundataPtr &rdata)
{
	u64 docid = pickDocid();
	u64 userid = pickUserid();
	rdata->setUserid(userid);
	bool results = false;
 	TestReadItr_t iter = mReadMap.find(docid);	
	TestWriteItr_t itw = mWriteMap.find(docid);
	if (iter != mReadMap.end() && itw == mWriteMap.end())
	{
		TestMap uu = iter->second;
		if (uu.count(userid) != 0)
		{
			results = true;
		}
	}
	//cout << "ReadUnLock" <<endl;
	printRead(rdata);
	//cout << "**********"<<endl;
	u64 lockid =0;
	if(results)lockid = mReadMap[docid][userid];
	bool rslt = AosDocLock::getSelf()->readUnLock(rdata, docid, lockid);
	aos_assert_r(rslt, false);
	if (!results)
	{
		aos_assert_r(rdata->getErrcode() == eAosXmlInt_General, false);
		return true;
	}
	//cout <<"UnLock :" << docid << ","<<userid << "," << lockid << endl;
	aos_assert_r(rdata->getErrcode() == eAosXmlInt_Ok, false);
	//mReadMap[docid].erase(userid);
	//aos_assert_r(mReadMap[docid].count(userid) == 0, false);
	iter->second.erase(userid);
	aos_assert_r(iter->second.count(userid) == 0, false);
	if (iter->second.size()== 0)
	{
		mReadMap.erase(docid);
		aos_assert_r(mReadMap.count(docid) == 0, false);
	}
	printRead(rdata);
	//cout << "ReadUnLock"<< endl;
	return true;
}

bool
AosDocLockTester::WriteLock(const AosRundataPtr &rdata)
{
	//cout <<"WriteLock" << endl;
	printRead(rdata);
	//cout <<"------" <<endl;
	printWrite(rdata);
	//cout << "**********"<<endl;
	u64 docid = pickDocid();
	u64 userid = pickUserid();
	rdata->setUserid(userid);
	bool results = false;
 	TestReadItr_t iter = mReadMap.find(docid);	
	TestWriteItr_t itw = mWriteMap.find(docid);
	if (iter == mReadMap.end() && itw == mWriteMap.end())
	{
		results = true;
	}
	u32 lock_timer = 1;//OmnGetSecond();
	bool rslt = AosDocLock::getSelf()->writeLock(rdata, docid, lock_timer);
	aos_assert_r(rslt, false);
	OmnString contents = rdata->getContents();
	if (!results)
	{
		if (rdata->getErrcode()!= eAosXmlInt_General)
		{
			cout << "read count:" << mReadMap.count(docid) << endl;
			cout << "write count:" << mWriteMap.count(docid) << endl;
			OmnMark;
		}
		aos_assert_r(rdata->getErrcode() == eAosXmlInt_General, false);
		return true;
	}
	aos_assert_r(rdata->getErrcode() == eAosXmlInt_Ok, false);
	AosXmlParser parser;
	AosXmlTagPtr contentsxml = parser.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_r(contentsxml, false);
	contentsxml = contentsxml->getFirstChild();
	aos_assert_r(contentsxml, false);
	contentsxml = contentsxml->getFirstChild();
	aos_assert_r(contentsxml, false);
	u64 lockid = contentsxml->getAttrU64(AOSTAG_LOCK_DID, 0);
	aos_assert_r(mWriteMap.count(docid) == 0 , false);
	mWriteMap.insert(make_pair(docid, lockid));
	printWrite(rdata);
	//cout <<"WriteLock" << endl;
	return true;
}

bool
AosDocLockTester::WriteUnLock(const AosRundataPtr &rdata)
{
	//cout <<"WriteUnLock" << endl;
	printRead(rdata);
	//cout <<"------" <<endl;
	printWrite(rdata);
	//cout << "**********"<<endl;
	u64 docid = pickDocid();
	u64 userid = pickUserid();
	rdata->setUserid(userid);
	bool results = false;
 	TestReadItr_t iter = mReadMap.find(docid);	
	TestWriteItr_t itw = mWriteMap.find(docid);
	if (iter == mReadMap.end() && itw != mWriteMap.end())
	{
		results = true;
	}
	u64 lockid = 0;
	if (results ) lockid = mWriteMap[docid];
	bool rslt = AosDocLock::getSelf()->writeUnLock(rdata, docid, lockid);
	aos_assert_r(rslt, false);
	if (!results)
	{
		aos_assert_r(rdata->getErrcode() == eAosXmlInt_General, false);
		return true;
	}
	aos_assert_r(rdata->getErrcode() == eAosXmlInt_Ok, false);
	aos_assert_r(mWriteMap.count(docid), false);
	mWriteMap.erase(docid);
	aos_assert_r(mWriteMap.count(docid) == 0, false);
	printWrite(rdata);
	//cout <<"WriteUnLock" << endl;
//timeout(rdata);
	return true;

}


u64
AosDocLockTester::pickNewDocid()
{
	u64 startdocid = 5000;
	u64 enddocid = 15000;
	u64 docid = (rand()%(enddocid - startdocid)) + startdocid;
	return docid;
}


u64
AosDocLockTester::pickDocid()
{
	int i = rand()%10;
	OmnString id = "123";
	id << i;
	return atoll(id);
}


u64
AosDocLockTester::pickUserid()
{
	int i = rand()%200;
	OmnString id = "888";
	id << i;
	return atoll(id);
}


bool
AosDocLockTester::printRead(const AosRundataPtr &rdata)
{
	return true;
	TestReadItr_t multitr;
	TestMapItr_t intertr ;
	for (multitr = mReadMap.begin(); multitr!= mReadMap.end(); multitr++)
	{
		for(intertr = multitr->second.begin();intertr != multitr->second.end(); intertr++)
		{
			cout <<"docid :"<< multitr->first <<" userid:"<< intertr->first<<"(lockid:" <<intertr->second<<")" << endl;
		}

	}
	return true;
}

bool
AosDocLockTester::printWrite(const AosRundataPtr &rdata)
{
	return true;
	TestMapItr_t itr;
	for (itr = mWriteMap.begin(); itr != mWriteMap.end(); itr++)
	{
		cout <<"docid: " <<itr->first << " lockid:"<<itr->second << endl;
	}
	return true;
}


bool
AosDocLockTester::timeout(const AosRundataPtr &rdata)
{
	mLock->lock();
	cout << "============================================ " 
		<<rdata->getArg1(AOSARG_LOCKED) << endl;
//	int vv = rand() %100;
//	if (vv < 50)
	{
//		smThread->waitLock();
///		smThread->wait();
//		smThread->waitUnlock();
		//AosDocLock::getSelf()->timeout(0, rdata);
		u64 docid = atoll(rdata->getArg1(AOSARG_DOCID));
		if ((AosStLockReq::REQ) atoll(rdata->getArg1(AOSARG_REQUEST_TYPE)) == AosStLockReq::eReadUnLockReq)
		{
			u64 userid = rdata->getUserid();
		 	TestReadItr_t iter = mReadMap.find(docid);	
			iter->second.erase(userid);
			aos_assert_r(iter->second.count(userid) == 0, false);
			if (iter->second.size()== 0)
			{
				mReadMap.erase(docid);
				aos_assert_r(mReadMap.count(docid) == 0, false);
			}
		}
		if ((AosStLockReq::REQ) atoll(rdata->getArg1(AOSARG_REQUEST_TYPE)) == AosStLockReq::eWriteUnLockReq)
		{
			aos_assert_r(mWriteMap.count(docid), false);
			mWriteMap.erase(docid);
			aos_assert_r(mWriteMap.count(docid) == 0, false);
		}
		//smThread->waitLock();
		//smThread->wakeup();
		//smThread->waitUnlock();
		mLock->unlock();
	}
	return true;
}

void
AosDocLockTester::waitThread()
{
	smThread->waitLock();
	smThread->wait();
	smThread->waitUnlock();
}

void
AosDocLockTester::signalThread()
{
	smThread->waitLock();
	smThread->wakeup();
	smThread->waitUnlock();
}
