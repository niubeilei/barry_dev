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
// This file is generated automatically by the ProgramAid facility.   
//
// Modification History:
// 08/27/2011: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "UserMgmt/UserDomain.h"

#include "API/AosApi.h"
#include "DocUtil/DocProcUtil.h"
#include "ErrorMgr/ErrmsgId.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/UserAcctObj.h"
#include "SEUtil/Objid.h"
#include "UserMgmt/UserAcctMgr.h"


static AosDocClientObjPtr sgDocClient;

AosUserDomain::AosUserDomain(
		const OmnString &ctnr_objid, 
		const AosUserAcctObjPtr &user_acct,
		const AosRundataPtr &rdata)
:
mUserAcct(user_acct),
mSupervisorFlag(true),
mIsSysDomain(false),
mCtnrObjid(ctnr_objid)
{
	if (!sgDocClient)
	{
		sgDocClient = AosDocClientObj::getDocClient();
		aos_assert(sgDocClient);
	}

	if (!mUserAcct)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateUserDomain);
		OmnThrowException("Failed creating user domain");
	}
	mIsSysDomain = mUserAcct->isSysAcct();

	// Ketty 2012/03/02
	getDomainDoc(rdata);
	aos_assert(mDomainDoc);
}


AosUserDomain::~AosUserDomain()
{
}


bool
AosUserDomain::isFriendDomain(const AosUserDomainObjPtr &domain)
{
	// Brian Zhang 2012/03/21 : up to 95
	return true;


	if (!domain) return false;
	if (mCtnrObjid == domain->getDomainObjid()) return true;
	if (mIsSysDomain || domain->isSysDomain()) return true;
	if (!isGood() || !domain->isGood()) return false;
	
	// Ketty 2012/03/02
	aos_assert_r(mDomainDoc && domain->getDomainDoc(0), false);
	OmnString friendDomains1 = mDomainDoc->getAttrStr(AOSTAG_FRIEND_DOMAINS);
	friendDomains1 << "," <<  mUserAcct->getFriendDomains();
	
	bool rslt = friendDomains1.hasCommonWords(domain->getDomainObjid(), ", ");
	if(!rslt)	return false;

	//OmnString friendDomains2 = domain->getFriendDomains();  
	OmnString friendDomains2 = domain->getDomainDoc(0)->getAttrStr(AOSTAG_FRIEND_DOMAINS);
	friendDomains2 << "," <<  domain->getFriendDomains();
	
	rslt = friendDomains2.hasCommonWords(mCtnrObjid, ", ");
	return rslt;
	
	//if (friendDomains1 == "" || friendDomains2 == "") return false;
	//return friendDomains1.hasCommonWords(friendDomains2, ", ");
}


AosXmlTagPtr 
AosUserDomain::getDomainDoc(const AosRundataPtr &rdata)
{
	aos_assert_r(sgDocClient, 0);
	if (mDomainDoc) return mDomainDoc;
	aos_assert_rr(mCtnrObjid != "", rdata, 0);
	mDomainDoc = sgDocClient->getDocByObjid(mCtnrObjid, rdata);
	aos_assert_rr(mDomainDoc, rdata, 0);
	return mDomainDoc;
}


AosXmlTagPtr
AosUserDomain::getUserDomainOprArd(const AosRundataPtr &rdata)
{
	aos_assert_r(sgDocClient, 0);
	// Each user domain has an Operation Access Record.
	AosXmlTagPtr domain_doc = getDomainDoc(rdata);
	aos_assert_rr(domain_doc, rdata, 0);

	OmnString docid = domain_doc->getAttrStr(AOSTAG_DOCID);
	OmnString objid = AosObjid::composeUserDomainOprArdObjid(docid);
	AosXmlTagPtr opr_ard = sgDocClient->getDocByObjid(objid, rdata);
	if (!opr_ard) return 0;

	aos_assert_r(opr_ard->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_DOMAIN_OPRARD, opr_ard);
	aos_assert_r(opr_ard->getAttrStr(AOSTAG_STYPE) == AOSSTYPE_DOMAIN_OPRARD, opr_ard);
	return opr_ard;
}


