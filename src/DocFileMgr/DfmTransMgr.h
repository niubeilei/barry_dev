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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 12/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmTransMgr_h
#define AOS_DocFileMgr_DfmTransMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

OmnDefineSingletonClass(AosDfmTransMgrSingleton,
                        AosDfmTransMgr,
                        AosDfmTransMgrSelf,
                        OmnSingletonObjId::eDfmTransMgr,
                        "DfmTransMgr");

class AosDfmTransMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
private:
	u64			mTransId;

	AosDfmTransMgr();
	~AosDfmTransMgr();

public:
    // Singleton class interface
    static AosDfmTransMgr*    getSelf();
    virtual bool        	start();
    virtual bool        	stop();
    virtual bool			config(const AosXmlTagPtr &def);
};
#endif

