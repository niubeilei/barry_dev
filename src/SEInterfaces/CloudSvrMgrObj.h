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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CloudSvrMgrObj_h
#define Aos_SEInterfaces_CloudSvrMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosCloudSvrMgrObj : virtual public OmnRCObject
{
private:
	static AosCloudSvrMgrObjPtr		smCloudSvrMgr;

public:
	virtual int getNumCloudServers() const = 0;

	static void setCloudSvrMgr(const AosCloudSvrMgrObjPtr &d) {smCloudSvrMgr = d;}
	static AosCloudSvrMgrObjPtr getCloudSvrMgr() {return smCloudSvrMgr;}
};

inline bool AoiGetNumCloudServers()
{
	AosCloudSvrMgrObjPtr obj = AosCloudSvrMgrObj::getCloudSvrMgr();
	aos_assert_r(obj, false);
	return obj->getNumCloudServers();
}
#endif
