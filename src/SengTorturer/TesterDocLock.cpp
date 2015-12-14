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
// 02/14/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterDocLock.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SengTorturer/TesterLogin.h"
#include "SengTorturer/SengTesterXmlDoc.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorturer/XmlTagRand.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "Util/UtUtil.h"


AosDocLockTester::AosDocLockTester(const bool regflag)
:
AosSengTester(AosSengTester_DocLock, AosTesterId::eDocLock, regflag),
mNum(0)
{
}


AosDocLockTester::AosDocLockTester()
:
AosSengTester(AosSengTester_DocLock, "doc_lock", AosTesterId::eDocLock),
mNum(0)
{
}


AosDocLockTester::~AosDocLockTester()
{
}


bool 
AosDocLockTester::test()
{
	mNum ++;
	if (mNum%10 == 0) 
	{
		while(1)
		{
			mThread->randomLogin();
			if (mThread->getCrtUserid() != 308) break; 
		}
	}

	aos_assert_r(pickDoc(), false);	
	if (!mRawDoc) return true;
	aos_assert_r(runLock(), false);
	if (mNum == 300)
	{
		TestReadItr_t multitr;
		TestMapItr_t intertr ;
		for (multitr = mReadMap.begin(); multitr!= mReadMap.end(); multitr++)
		{
			for(intertr = multitr->second.begin();intertr != multitr->second.end(); intertr++)
			{
				OmnScreen << "ReadLock ---- " <<"docid :"<< multitr->first <<" userid:"<< intertr->first<<"(lockid:" <<intertr->second<<")" << endl;
			}

		}

		TestMapItr_t itr;
		for (itr = mWriteMap.begin(); itr != mWriteMap.end(); itr++)
		{
			    OmnScreen << "WriteLock ---- " <<"docid: " <<itr->first << " lockid:"<<itr->second << endl;
		}
		return true;
	}
	return true;
}


bool
AosDocLockTester::pickDoc()
{
	mLocalDocid = 0;
	mServerDocid = 0;
	mObjid = "";
	mResp = 0;
	mRawDoc = 0;
	int i = 1;
	int total = mThread->getStDocSize();
	while (i++<total)
	{
		AosStDocPtr doc = mThread->pickDoc();
		if (!doc) continue;
		mObjid = doc->objid;
		mLocalDocid = doc->locdid;
		mServerDocid = doc->svrdid;
		AosStContainerPtr container = mThread->getContainer(mLocalDocid);
		AosStUser *user = mThread->getUser(mLocalDocid);
		const AosStAccessRecord *ar = mThread->getLocalAccessRecord(mLocalDocid);
		if (!container && !user && !ar)
		{
			//Not allow delete container and user
	         break;
        }
     }
		
	if (i >= total) return true;
	mServerDoc = mThread->retrieveDocByObjid(mObjid);
	if (!mServerDoc) return true;
	mRawDoc = mThread->readLocalDocFromFile(mLocalDocid);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OBJID, "") == mObjid, false);
	aos_assert_r(mServerDoc->getAttrU64(AOSTAG_DOCID, 0) == mServerDocid, false);
	return true;
}

bool
AosDocLockTester::runLock()
{
	int vv = rand()%100;
	if (vv < 25)
	{
OmnScreen << "readlock: " << mServerDocid<< endl;
		return ReadLock();
	}

	if (vv < 50)
	{
OmnScreen << "ReadUnLock:" << mServerDocid<< endl;
		return ReadUnLock();
	}

	if (vv < 75)
	{
OmnScreen << "WriteLock:" << mServerDocid << endl;
		return WriteLock();
	}
OmnScreen << "WriteUnLock:" << mServerDocid << endl;
	WriteUnLock();
	return true;
}


bool
AosDocLockTester::ReadLockReq()
{
	OmnString req = "<doclock ";
	req << AOSTAG_LOCK_TYPE << "=\"" << "readlock" << "\" "
		<< AOSTAG_OBJID << "=\""<< mObjid << "\" "
		<< AOSTAG_LOCK_TIMER << "=\"" << "30" << "\" />";

	sendDocLockReq(req);
	return true;
}

bool
AosDocLockTester::ReadLock()
{
	ReadLockReq();

	u64 userid = mThread->getCrtUserid();
	u64 docid = mServerDocid;
	bool results = false;
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

	aos_assert_r(mResp, false);
	if (!results)
	{
		aos_assert_r(mResp->getAttrStr("zky_status", "") == "failure", false);
		return true;
	}


	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);
	u64 lockid = mResp->getAttrU64(AOSTAG_LOCK_DID, 0);
	aos_assert_r(lockid, false);
	if (mReadMap.count(docid)== 0)
	{
		mReadMap[docid][userid] = lockid;
		aos_assert_r(mReadMap[docid][userid] == lockid, false);
	}
	else
	{
		aos_assert_r(mReadMap[docid].count(userid)==0, false) ;
		mReadMap[docid][userid] = lockid;
		aos_assert_r(mReadMap[docid][userid] == lockid, false);
	}
	aos_assert_r(mReadMap[docid][userid] == lockid, false);

	aos_assert_r(CheckReadLock(), false);
	return  true;
}

bool
AosDocLockTester::CheckReadLock()
{
	//Check Read Lock
	//login user ,Allowed to read  
	aos_assert_r(AosSengAdmin::getSelf()->retrieveDocByDocid(
						mThread->getSiteid(), 
						mThread->getCrtSsid(), 
						mThread->getUrlDocid(),
						mServerDocid, 
						false), false);

	// login user ,Not allowed to write
	aos_assert_r(mServerDoc, false);
	int value = mServerDoc->getAttrInt("zky_aabbcc", 0);
	mServerDoc->setAttr("zky_aabbcc" , value + 1);

	bool rslt = mThread->modifyObj(mServerDoc, false); 
	aos_assert_r(!rslt, false);
	return true;
}

bool
AosDocLockTester::WriteLockReq()
{
	OmnString req = "<doclock ";
	req << AOSTAG_LOCK_TYPE << "=\"" << "writelock" << "\" "
		<< AOSTAG_OBJID << "=\""<< mObjid << "\" "
		<< AOSTAG_LOCK_TIMER << "=\"" << "30" << "\" />";

	sendDocLockReq(req);
	return true;
}
bool
AosDocLockTester::WriteLock()
{
	WriteLockReq();

	u64 docid = mServerDocid;
	bool results = false;
	TestReadItr_t iter = mReadMap.find(docid);
	TestWriteItr_t itw = mWriteMap.find(docid);
	if (iter == mReadMap.end() && itw == mWriteMap.end())
	{
		results = true;
	}
	
	aos_assert_r(mResp, false);
	if (!results)
	{
		aos_assert_r(mResp->getAttrStr("zky_status", "") == "failure", false);
		return true;
	}
	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);
	u64 lockid = mResp->getAttrU64(AOSTAG_LOCK_DID, 0);
	aos_assert_r(lockid, false);
	aos_assert_r(mWriteMap.count(docid) == 0 , false);
	mWriteMap.insert(make_pair(docid, lockid));

	aos_assert_r(CheckWriteLock(), false);
	return true;
}

bool
AosDocLockTester::CheckWriteLock()
{
	// login user, Allowed to read|write  
	AosXmlTagPtr dd = AosSengAdmin::getSelf()->retrieveDocByDocid(
						mThread->getSiteid(), 
						mThread->getCrtSsid(), 
						mThread->getUrlDocid(),
						mServerDocid, 
						true
						);
	aos_assert_r(dd, false);

	aos_assert_r(mServerDoc, false);
	int value = mServerDoc->getAttrInt("zky_aabbcc", 0);
	mServerDoc->setAttr("zky_aabbcc" , value + 1);

	bool rslt = mThread->modifyObj(mServerDoc, false);
	aos_assert_r(rslt, false);

	//Other users, ,Not allowed to write 
	mServerDoc->setAttr("zky_aabbcc" , value + 2);

	OmnString ssid = "";
	OmnString data = mServerDoc->toString();
	rslt = AosSengAdmin::getSelf()->sendModifyReq(
			        mThread->getSiteid(), 
			        ssid, 
					mThread->getUrlDocid(), 
			        data, 
					mThread->getRundata(), 
					false);
	aos_assert_r(!rslt, false);

	dd = AosSengAdmin::getSelf()->retrieveDocByDocid(
					mThread->getSiteid(), 
					"",
					mThread->getUrlDocid(), 
					mServerDocid, 
					true);
	aos_assert_r(!dd, false);
	return true;
}

bool
AosDocLockTester::ReadUnLockReq(const u64 &lockid)
{
	OmnString req = "<doclock ";
	req << AOSTAG_LOCK_TYPE << "=\"" << "readunlock" << "\" "
		<< AOSTAG_OBJID << "=\""<< mObjid << "\" "
		<< AOSTAG_LOCK_DID << "=\"" << lockid << "\" />";

	sendDocLockReq(req);
	return true;
}


bool
AosDocLockTester::ReadUnLock()
{
	u64 userid = mThread->getCrtUserid();
	u64 docid = mServerDocid;

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

	u64 lockid = 0;
	if(results)lockid = mReadMap[docid][userid];
	
	if (lockid == 0) lockid = atoll(AosGeneratePasswd().data());

	ReadUnLockReq(lockid);
	if (!results)
	{
		aos_assert_r(mResp->getAttrStr("zky_status", "") == "failure", false);
		return true;
	}

	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);
	iter->second.erase(userid);
	aos_assert_r(iter->second.count(userid) == 0, false);
	if (iter->second.size()== 0)
	{
	    mReadMap.erase(docid);
	    aos_assert_r(mReadMap.count(docid) == 0, false);
	}
	aos_assert_r(CheckReadUnLock(), false);
	return true;
}

bool
AosDocLockTester::CheckReadUnLock()
{
	//Check Read Unlock
	// login user, Allowed to read
	AosXmlTagPtr dd = AosSengAdmin::getSelf()->retrieveDocByDocid(
							mThread->getSiteid(), 
							mThread->getCrtSsid(),
							mThread->getUrlDocid(), 
							mServerDocid, 
							false);
	aos_assert_r(dd, false);

	// login user, Allowed to read lock lock
	ReadLockReq();
	aos_assert_r(mResp, false);
	u64 lockid = mResp->getAttrU64("zky_lock_did", 0);
	aos_assert_r(lockid, false);

	ReadUnLockReq(lockid);
	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);
	return true;
}

bool
AosDocLockTester::WriteUnLockReq(const u64 &lockid)
{
	OmnString req = "<doclock ";
	req << AOSTAG_LOCK_TYPE << "=\"" << "writeunlock" << "\" "
		<< AOSTAG_OBJID << "=\""<< mObjid << "\" "
		<< AOSTAG_LOCK_DID << "=\"" << lockid << "\" />";

	sendDocLockReq(req);
	return true;
}

bool
AosDocLockTester::WriteUnLock()
{
	u64 docid = mServerDocid;

	bool results = false;
	TestReadItr_t iter = mReadMap.find(docid);
	TestWriteItr_t itw = mWriteMap.find(docid);
	if (iter == mReadMap.end() && itw != mWriteMap.end())
	{
	    results = true;
	}
	u64 lockid = 0;
	if (results ) lockid = mWriteMap[docid];
	if (lockid == 0) lockid = atoll(AosGeneratePasswd().data());

	WriteUnLockReq(lockid);

	if (!results)
	{
	    aos_assert_r(mResp->getAttrStr("zky_status", "") == "failure", false);
	    return true;
	}

	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);
	aos_assert_r(mWriteMap.count(docid), false);
	mWriteMap.erase(docid);
	aos_assert_r(mWriteMap.count(docid) == 0, false);
	aos_assert_r(CheckWriteUnLock(), false);
	return true;
}

bool
AosDocLockTester::CheckWriteUnLock()
{
	//Check Read Unlock
	// login user, Allowed to read|write
	AosXmlTagPtr dd = AosSengAdmin::getSelf()->retrieveDocByDocid(
							mThread->getSiteid(), 
							mThread->getCrtSsid(),
							mThread->getUrlDocid(), 
							mServerDocid, 
							false);
	aos_assert_r(dd, false);

	aos_assert_r(mServerDoc, false);
	int value = mServerDoc->getAttrInt("zky_aabbcc", 0);
	mServerDoc->setAttr("zky_aabbcc" , value + 1);

	bool rslt = mThread->modifyObj(mServerDoc, false);
	aos_assert_r(rslt, false);


	// Other users,  Allowed to read|read
	OmnString ssid = "";
	dd = AosSengAdmin::getSelf()->retrieveDocByDocid(
							mThread->getSiteid(), 
							ssid,
							mThread->getUrlDocid(), 
							mServerDocid, 
							false);
	aos_assert_r(dd, false);


	aos_assert_r(mServerDoc, false);
	value = mServerDoc->getAttrInt("zky_aabbcc", 0);
	mServerDoc->setAttr("zky_aabbcc" , value + 2);

	OmnString data = mServerDoc->toString();
	rslt = AosSengAdmin::getSelf()->sendModifyReq(
				mThread->getSiteid(), 
				ssid, 
				mThread->getUrlDocid(), 
				data,
				mThread->getRundata(), 
				false);
	aos_assert_r(rslt, false);


	// login user, Allowed to read lock|write lock
	ReadLockReq();
	aos_assert_r(mResp, false);
	u64 lockid = mResp->getAttrU64("zky_lock_did", 0);
	aos_assert_r(lockid, false);

	ReadUnLockReq(lockid);
	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);

	WriteLockReq();
	aos_assert_r(mResp, false);
	lockid = mResp->getAttrU64("zky_lock_did", 0);
	aos_assert_r(lockid, false);

	WriteUnLockReq(lockid);
	aos_assert_r(mResp->getAttrStr("zky_status", "") == "success", false);
	return true;
}


bool
AosDocLockTester::sendDocLockReq(const OmnString &req)
{
	mResp = AosSengAdmin::getSelf()->sendDocLockReq(
			mThread->getSiteid(), 
			mThread->getCrtSsid(), 
			mThread->getUrlDocid(), 
			req);
	return true;
}
