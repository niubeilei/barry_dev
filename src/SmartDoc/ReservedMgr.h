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
#ifndef AOS_SmartDoc_ReservedMgr_h
#define AOS_SmartDoc_ReservedMgr_h

#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include <map>
using namespace std;

OmnDefineSingletonClass(AosReservedMgrSingleton,
						AosReservedMgr,
						AosReservedMgrSelf,
						OmnSingletonObjId::eReservedMgr,
						"AosReservedMgr");

class AosReservedMgr: virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	map<u64, int>		mTimers;
	OmnMutexPtr         mLock;

public:
	AosReservedMgr();
	~AosReservedMgr();

    //
    // Singleton class interface
    //
    static AosReservedMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	void 	addTimer(const int timerid, const u64 &docid);
	void	cancelTimer(const u64 &docid);
	void	removeTimer(const u64 &docid);

};
#endif
