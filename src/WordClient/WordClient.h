////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_WordClient_WordClient_h
#define AOS_WordClient_WordClient_h

#include "SearchEngine/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/File.h"


OmnDefineSingletonClass(AosWordClientSingleton,
						AosWordClient,
						AosWordClientSelf,
						OmnSingletonObjId::eWordClient,
						"WordClient");

class AosWordClient : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:

public:
	AosWordClient();
	~AosWordClient();

	// OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread){return false;}
    virtual bool    signal(const int threadLogicId) {return false;}
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const {return false;}

	u64		addWord(const char *word, const int len, const u64 &ptr);
	u64		getWordId(const OmnString &word, const bool addFlag);
	u64		getWordId(const u8 *word, const bool addFlag)
			{return getWordId(word, strlen((char *)word), addFlag);}
	u64		getWordId(const u8 *word, 
				const int len, 
				const bool addFlag = true);
	char *	getWord(const u64 &wordId, u32 &len);
	// bool 	start(const OmnString &wordIdName, const OmnString &, const u32 tsize);
	bool	getPtr(const u64 &wordId, u64 &ptr);
	bool	setPtr1(const u64 &wordId, const u64 &ptr);
	bool	createHashtable();

    // Singleton class interface
    static AosWordClient *    	getSelf();
    virtual bool            start();
    virtual bool            stop();
    virtual bool			config(const AosXmlTagPtr &def);


	//ken 2011/07/12
	bool	addWord(
				const char *word,
				const int len,
				const u64 &wordid,
				const u64 &ptr);
	
};

#endif
#endif
