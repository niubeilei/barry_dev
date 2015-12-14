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
// 2015/10/15 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SyncEngineClient_h
#define Aos_SEInterfaces_SyncEngineClient_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SyncEngineClientObj.h"
#include "SEInterfaces/SyncherType.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


OmnDefineSingletonClass(AosSyncEngineClientSingleton,
                        AosSyncEngineClient,
                        AosSyncEngineClientSelf,
                        OmnSingletonObjId::eSyncEngineClient,
                        "SyncEngineClient");


class AosSyncEngineClient : public AosSyncEngineClientObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr         				mLock;
	map<OmnString, AosSyncherObjPtr> 	mSynchersMap;

	// singleton 
	AosSyncEngineClient();
	~AosSyncEngineClient();

public:
    // Singleton class interface
    static AosSyncEngineClient* getSelf();
    virtual bool				config(const AosXmlTagPtr &def);
    virtual bool        		start();
    virtual bool        		stop();

	// Jimo interface 
	virtual AosJimoPtr cloneJimo() const;

	// SyncEngineClinet interface
	virtual bool isTableSynced(AosRundata *rdata, const OmnString &tablename);
	virtual AosSyncherObjPtr getSyncher(AosRundata *rdata, 
						const OmnString &sync_name,
						const AosSyncherType::E type);

private:
	OmnString repairSyncherName(AosRundata *rdata,
						const OmnString &sync_name,
						const AosSyncherType::E type);
};
#endif

