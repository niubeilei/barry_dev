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
// Modification History:
// 01/31/2012: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UserMgmt_UserMgmt_h
#define Aos_UserMgmt_UserMgmt_h

#include "SEInterfaces/UserMgmtObj.h"
#include "UserMgmt/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosUserMgmt : virtual public AosUserMgmtObj
{
	OmnDefineRCObject;

public:
    AosUserMgmt();
    ~AosUserMgmt();

	virtual AosUserDomainObjPtr getUserDomain1(
					const OmnString &objid, 
					const AosUserAcctObjPtr &user_acct, 
					const AosRundataPtr &rdata);

	virtual AosUserAcctObjPtr getUserAcctByCloudid(
					const OmnString &cid, 
					const AosRundataPtr &rdata);

	virtual AosUserAcctObjPtr getUserAcctByDocid(
					const u64 &docid, 
					const AosRundataPtr &rdata);

	virtual AosUserAcctObjPtr getUserAcct(
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata);

	virtual bool checkManagership(
					const AosUserAcctObjPtr &requester,
					const AosUserAcctObjPtr &user,
					const OmnString &aname,
					const bool direct_mgr,
					const AosRundataPtr &rdata);
};
#endif

