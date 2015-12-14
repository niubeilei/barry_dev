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
// 2011/07/19	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WordTransClient_WordTransClient_h
#define AOS_WordTransClient_WordTransClient_h

#include "SearchEngine/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include <deque>

OmnDefineSingletonClass(AosWordTransClientSingleton,
						AosWordTransClient,
						AosWordTransClientSelf,
						OmnSingletonObjId::eWordTransClient,
						"WordTransClient");

class AosWordTransClient : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eNumIILsPerRequest = 100
	};

private:
	AosTransClientPtr	mTransClient;
	bool				mIsLocal;
	deque<u64>			mCreatedIILs[eAosIILType_Total];
	OmnMutexPtr			mLock;


public:
	AosWordTransClient();
	~AosWordTransClient();

    // Singleton class interface
    static AosWordTransClient *    	getSelf();
    virtual bool            start();
    virtual bool            stop();
    virtual bool			config(const AosXmlTagPtr &def);

	// WordTransClient Interface
	u64		getIILID(
				const OmnString &word,
				const bool addFlag,
				const bool isPersis,
				const AosIILType &iiltype,
				const AosRundataPtr &rdata)
			{
				return getIILID(word.data(), word.length(), addFlag, isPersis, iiltype, rdata);
			}

	u64		getIILID(
				const char *word,
				const bool addFlag,
				const bool isPersis,
				const AosIILType &iiltype,
				const AosRundataPtr &rdata)
			{
				return getIILID(word, strlen(word), addFlag, isPersis, iiltype, rdata);
			}

	u64		getIILID(
				const char *word,
				const int len,
				const bool addFlag,
				const bool isPersis,
				const AosIILType &iiltype,
				const AosRundataPtr &rdata);

	u64		getRemoteIILID(
				const char *word,
				const int len,
				const bool addFlag,
				const bool isPersis,
				const AosIILType &iiltype,
				const AosRundataPtr &rdata);

	u64		getLocalIILID(
				const char *word,
				const int len,
				const bool addFlag,
				const bool isPersis,
				const AosIILType &iiltype,
				const AosRundataPtr &rdata);
	
	u64		getIILID(
				const char *word,
				const int len,
				const AosRundataPtr &rdata);

	u64		getIILID(const OmnString &word, const AosRundataPtr &rdata)
			{
				return getIILID(word.data(), word.length(), rdata);
			}

	bool getLocalIILIDs( 
				const AosIILType &iiltype,
				const AosRundataPtr &rdata);
};

#endif
