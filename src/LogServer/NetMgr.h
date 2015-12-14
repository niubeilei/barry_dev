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
//////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogServer_NetMgr_h
#define AOS_LogServer_NetMgr_h

#include "LogServer/Log.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosNetMgr  : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual  AosLogPtr	getLog(const AosNetRequestPtr &request) = 0;
};
#endif

