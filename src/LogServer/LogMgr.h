//////////////////////////////////////////////////////////////////////////
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
// 03/20/2009: Created by Sharon Shen
// 01/01/2013: Turned off by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_LogServer_LogMgr_h
#define AOS_LogServer_LogMgr_h

#include "LogServer/Log.h"
#include "LogServer/NetMgr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#include <list>

class AosLogMgr : virtual public AosNetMgr 
{
	OmnDefineRCObject;

protected:
	std::list<AosLogPtr> mLogList;

public:
    AosLogMgr();
    ~AosLogMgr();

	AosLogPtr getLog(const AosNetRequestPtr &request);
	AosLogPtr createLog(const AosNetRequestPtr &request);

private:
	AosLogPtr getInstanceLog(const AosLogPtr &theLog, const AosNetRequestPtr &request);

};
#endif

#endif
