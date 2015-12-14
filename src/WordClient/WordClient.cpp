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
// 11/16/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "WordClient/WordClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "IdGen/IdGen.h"
#include "Porting/Sleep.h"
#include "SEUtil/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "WordMgr/WordMgr1.h"

OmnSingletonImpl(AosWordClientSingleton,
                 AosWordClient,
                 AosWordClientSelf,
                "AosWordClient");


AosWordClient::AosWordClient()
{
}


AosWordClient::~AosWordClient()
{
}


// bool 	
// AosWordClient::start(const OmnString &wordIdName, const OmnString &fname, const u32 tsize)
// {
// 	return AosWordMgr1::getSelf()->start(wordIdName, fname, tsize);
// }


bool
AosWordClient::start()
{
	return true;
}


bool
AosWordClient::config(const AosXmlTagPtr &config)
{
	return true;
}


u64
AosWordClient::getWordId(
		const OmnString &word, 
		const bool addFlag)
{
	return AosWordMgr1::getSelf()->getWordId(word, addFlag);
	/*
	u64 wordid = AosWordMgr1::getSelf()->getWordId(
			(const u8*)word.data(), word.length(), false);
	if (!addFlag || wordid != AOS_INVWID) return wordid;

	// No wordid was created for the word yet. Need to send a request
	// to the word server. 
	OmnNotImplementedYet;
	return 0;
	*/
}


u64
AosWordClient::getWordId(const u8 *word, const int len, const bool addFlag)
{
	return AosWordMgr1::getSelf()->getWordId(word, len, addFlag);
	/*
	u64 wordid = AosWordMgr1::getSelf()->getWordId(word, len, false);
	if (!addFlag || wordid != AOS_INVWID) return wordid;

	// No wordid was created for the word yet. Need to send a request
	// to the word server. 
	OmnNotImplementedYet;
	return 0;
	*/
}


bool
AosWordClient::getPtr(const u64 &wordId, WordPtr &ptr)
{
	bool rslt = AosWordMgr1::getSelf()->getPtr(wordId, ptr);
	return rslt;
}


bool
AosWordClient::setPtr1(const u64 &wordId, const WordPtr &ptr)
{
	bool rslt = AosWordMgr1::getSelf()->setPtr1(wordId, ptr);
	return rslt;
}


char *	
AosWordClient::getWord(const u64 &wordId, u32 &len)
{
	return AosWordMgr1::getSelf()->getWord(wordId, len);
}


bool	
AosWordClient::savePtr1(const u64 &wordId, const WordPtr &ptr)
{
	bool rslt = AosWordMgr1::getSelf()->savePtr1(wordId, ptr);
	return rslt;
}


u64		
AosWordClient::addWord(const char *word, const int len, const WordPtr &ptr)
{
	return AosWordMgr1::getSelf()->addWord(word, len, ptr);
}


bool
AosWordClient::stop()
{

	return AosWordMgr1::getSelf()->stop();
}


bool
AosWordClient::addWord(
		const char *word,
		const int len,
		const u64 &wordid,
		const u64 &ptr)
{
	return AosWordMgr1::getSelf()->addWord(word, len, wordid, ptr);
}

#endif
