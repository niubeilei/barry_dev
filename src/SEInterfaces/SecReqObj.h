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
// 02/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SecReqObj_h
#define Aos_SEInterfaces_SecReqObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSecReqObj : virtual public OmnRCObject
{
private:
	static AosSecReqObjPtr		smSecReq;

public:
	virtual OmnString		getRequesterDomains() const = 0;
	virtual OmnString		getRequesterUserRoles() const = 0;
	virtual OmnString		getRequesterUserGroups() const = 0;

	static void setSecReq(const AosSecReqObjPtr &d) {smSecReq = d;}
	static AosSecReqObjPtr getSecReq() {return smSecReq;}
};

inline OmnString AosGetRequesterDomains()
{
	AosSecReqObjPtr req = AosSecReqObj::getSecReq();
	aos_assert_r(req, "");
	return req->getRequesterDomains();
}

inline OmnString AosGetRequesterUserRoles()
{
	AosSecReqObjPtr req = AosSecReqObj::getSecReq();
	aos_assert_r(req, "");
	return req->getRequesterUserRoles();
}

inline OmnString AosGetRequesterUserGroups()
{
	AosSecReqObjPtr req = AosSecReqObj::getSecReq();
	aos_assert_r(req, "");
	return req->getRequesterUserGroups();
}

#endif
