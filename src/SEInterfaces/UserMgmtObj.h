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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_UserMgmtObj_h
#define Aos_SEInterfaces_UserMgmtObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosUserMgmtObj : virtual public OmnRCObject
{
private:
	static AosUserMgmtObjPtr smUserMgmt;

public:
	virtual AosUserDomainObjPtr getUserDomain1(
					const OmnString &objid, 
					const AosUserAcctObjPtr &user_acct, 
					const AosRundataPtr &rdata) = 0;

	virtual AosUserAcctObjPtr getUserAcctByCloudid(
					const OmnString &cid, 
					const AosRundataPtr &rdata) = 0;

	virtual AosUserAcctObjPtr getUserAcctByDocid(
					const u64 &docid, 
					const AosRundataPtr &rdata) = 0;

	virtual AosUserAcctObjPtr getUserAcct(
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata) = 0;

	// Chen Ding, 12/30/2012
	virtual bool checkManagership(
					const AosUserAcctObjPtr &requester,
					const AosUserAcctObjPtr &user,
					const OmnString &aname,
					const bool direct_mgr,
					const AosRundataPtr &rdata) = 0;

	static AosUserMgmtObjPtr getUserMgmt() {return smUserMgmt;}
	static void setUserMgmt(const AosUserMgmtObjPtr &d) {smUserMgmt = d;}
};
#endif
