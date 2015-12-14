////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DistributedFileMgrObj_h
#define Aos_SEInterfaces_DistributedFileMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosDistributedFileMgrObj : virtual public OmnRCObject
{
private:
	static AosDistributedFileMgrObjPtr		smDistributedFileMgr;

public:
	static void setDistributedFileMgr(const AosDistributedFileMgrObjPtr &d) {smDistributedFileMgr = d;}
	static AosDistributedFileMgrObjPtr getDistributedFileMgr() {return smDistributedFileMgr;}
};
#endif
