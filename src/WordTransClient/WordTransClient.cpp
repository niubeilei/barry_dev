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
// This is a wrap to the WordMgr. When someone wants a wordid, it calls
// a member function of this class. The class checks whether the wordid
// has been created. If yes, it returns the wordid. Otherwise, it sends
// a message to one of the WordServer. After receiving the response, 
// it will update its database for the new binding. 
//
// Modification History:
// 2011/07/19	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "WordTransClient/WordTransClient.h"

#include "IILClient/IILClient.h"
#include "IILUtil/IILFuncType.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TransClient/TransClient.h"
#include "TransUtil/RoundRobin.h"
#include "Thread/Mutex.h"
#include "WordMgr/WordMgr1.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosWordTransClientSingleton,
                 AosWordTransClient,
                 AosWordTransClientSelf,
                "AosWordTransClient");


AosWordTransClient::AosWordTransClient()
:
mIsLocal(true),
mLock(OmnNew OmnMutex())
{
}


AosWordTransClient::~AosWordTransClient()
{
}


bool
AosWordTransClient::start()
{
	return true;
}


bool
AosWordTransClient::stop()
{
	return true;
}


bool 	
AosWordTransClient::config(const AosXmlTagPtr &config)
{
	AosXmlTagPtr wordTrans = config->getFirstChild("word_client");
	if(!wordTrans) return false;
	
	mIsLocal = wordTrans->getAttrBool("isLocal", false);
	if(!mIsLocal)
	{
		AosTransDistributorPtr roundRobin = OmnNew AosRoundRobin();
		mTransClient = OmnNew AosTransClient(wordTrans, roundRobin);
	}
	return true;
}


u64
AosWordTransClient::getIILID( 
		const char *word,
		const int len,
		const bool addFlag,
		const bool isPersis,
		const AosIILType &iiltype,
		const AosRundataPtr &rdata)
{
	aos_assert_r(AosIsValidIILType(iiltype), 0);
	u64 iilid = getIILID(word, len, rdata);
	if(iilid) return iilid;
	if(!addFlag) return 0;

	// Need to create new one. Check whether it has created ones.
	mLock->lock();
	if (mIsLocal)
	{
		if (mCreatedIILs[iiltype].size() <= 0)
		{
			//OmnThreadPtr thread = OmnThreadMgr::getCurrentThread();
			//AosThreadAppendLog(thread, "");
			bool rslt = getLocalIILIDs(iiltype, rdata);
			//AosThreadAppendLog(thread, "");
			aos_assert_rl(rslt, mLock, 0);
		}
		aos_assert_rl(mCreatedIILs[iiltype].size() > 0, mLock, 0);
		iilid = mCreatedIILs[iiltype].front();
		mCreatedIILs[iiltype].pop_front();
		u64 wordid = AosWordMgr1::getSelf()->addWord(word, len, iilid << 32);
		mLock->unlock();
		aos_assert_r(wordid, 0);
		return iilid;
	}

	iilid = getRemoteIILID(word, len, addFlag, isPersis, iiltype, rdata);
	mLock->unlock();
	aos_assert_r(iilid > 0, 0);
	return iilid;
}


u64
AosWordTransClient::getLocalIILID( 
		const char *word,
		const int len,
		const bool addFlag,
		const bool isPersis,
		const AosIILType &iiltype,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	bool rslt = AosIILClient::getSelf()->createIILPublic(iilid, iiltype, isPersis, rdata);
	aos_assert_r(rslt, 0);
	aos_assert_r(iilid, 0);

	u64 wordid = AosWordMgr1::getSelf()->addWord(word, len, iilid << 32);
	aos_assert_r(wordid, 0);
	return iilid;
}


u64
AosWordTransClient::getRemoteIILID( 
		const char *word,
		const int len,
		const bool addFlag,
		const bool isPersis,
		const AosIILType &iiltype,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mTransClient, 0);

	OmnString docstr;
	OmnString ww(word, len);
	docstr << "<req operation=\"" << AosIILFuncType::eGetIILID << "\" "
	   	   << "word=\"" << ww << "\" "
		   << "iiltype=\"" << AosIILType_toStr(iiltype) << "\" "
		   << "addFlag=\"" << addFlag << "\" "
		   << "isPersis=\"" << isPersis << "\" />";

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, 0);
		
	AosXmlTagPtr rsp;
	bool timeout = false;
	bool rslt = mTransClient->addTrans(xml, rsp, timeout, rdata, 0);
	if (timeout)
	{
		rdata->setError() << "Failed to get iilid: " << word;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	aos_assert_r(rslt || rsp, 0);

	u64 wordid = rsp->getAttrU64("wordid", 0);
	if(wordid == 0)
	{
		if(addFlag) OmnAlarm << "failed to get wordid" << enderr;
		return 0;
	}

	u64 iilid = rsp->getAttrU64("iilid", 0);
	aos_assert_r(iilid, 0);
	
	u64 ptr = iilid << 32;
	rslt = AosWordMgr1::getSelf()->addWord(word, len, wordid, ptr);
	aos_assert_r(rslt, 0);
	return iilid;
}


u64
AosWordTransClient::getIILID(
		const char *word,
		const int len,
		const AosRundataPtr &rdata)
{
	u64 wordid = AosWordMgr1::getSelf()->getWordId((u8 *)word, len, false);
	if (!wordid) return 0;
	
	u64 ptr = 0;
	bool rslt = AosWordMgr1::getSelf()->getPtr(wordid, ptr);
	aos_assert_r(rslt, 0);
	return ptr >> 32;
}


/*
bool
AosWordTransClient::getLocalIILIDs( 
		const AosIILType &iiltype,
		const AosRundataPtr &rdata)
{
	bool rslt = AosIILClient::getSelf()->createIILsPublic(
			mCreatedIILs[iiltype], eNumIILsPerRequest, iiltype, rdata);
	aos_assert_r(rslt, false);
	return true;
}
*/
