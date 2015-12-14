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
#include "UserMgmt/UserMgmt.h"

#include "SEInterfaces/UserDomainObj.h"
#include "UserMgmt/UserDomainMgr.h"
#include "UserMgmt/UserAcctMgr.h"
#include "XmlUtil/XmlTag.h"


AosUserMgmt::AosUserMgmt()
{
}


AosUserMgmt::~AosUserMgmt()
{
}


AosUserDomainObjPtr 
AosUserMgmt::getUserDomain1(
		const OmnString &objid, 
		const AosUserAcctObjPtr &user_acct, 
		const AosRundataPtr &rdata)
{
	return AosUserDomainMgr::getSelf()->getUserDomain1(objid, user_acct, rdata);
}


AosUserAcctObjPtr 
AosUserMgmt::getUserAcctByCloudid(const OmnString &cid, const AosRundataPtr &rdata)
{
	return AosUserAcctMgr::getSelf()->getUserAcctByCloudid(cid, rdata);
}


AosUserAcctObjPtr 
AosUserMgmt::getUserAcctByDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	return AosUserAcctMgr::getSelf()->getUserAcctByDocid(docid, rdata);
}


AosUserAcctObjPtr 
AosUserMgmt::getUserAcct(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	return AosUserAcctMgr::getSelf()->getUserAcct(doc, rdata);
}


bool 
AosUserMgmt::checkManagership(
		const AosUserAcctObjPtr &requester,
		const AosUserAcctObjPtr &user,
		const OmnString &aname,
		const bool direct_mgr,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

