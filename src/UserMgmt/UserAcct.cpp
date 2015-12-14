////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UserMgmt/UserAcct.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/UserDomainObj.h"
#include "SEUtil/ValueDefs.h"
#include "SEBase/SecUtil.h"
#include "XmlUtil/SeXmlUtil.h"


AosUserAcct::AosUserAcct(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
:
mDoc(doc),
mUserOprArdTried(false)
{
}


bool 
AosUserAcct::isRoot(
		const AosUserDomainObjPtr &accessed_domain,
		const AosUserDomainObjPtr &requester_domain) const
{
	// This function returns true if this account is the accessed
	// domain's admin.
	if (!mDoc) return false;
	if (!accessed_domain || !requester_domain) return false;
	if (!accessed_domain->isFriendDomain(requester_domain)) return false;
	return isRoot();
}


bool 
AosUserAcct::isAdmin(
		const AosUserDomainObjPtr &accessed_domain,
		const AosUserDomainObjPtr &requester_domain) const
{
	// This function returns true if this account is the accessed
	// domain's admin.
	if (!mDoc) return false;
	if (!accessed_domain || !requester_domain) return false;
	if (!accessed_domain->isFriendDomain(requester_domain)) return false;
	return isAdmin();
}

/*
bool 
AosUserAcct::isUserAcct(const AosXmlTagPtr &doc)
{
	if (!doc) return false;
	return doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT;
}
*/

bool 
AosUserAcct::isSupervisor(
		const AosUserAcctObjPtr &supervisor_acct, 
		const AosUserDomainObjPtr &self_domain, 
		const AosUserDomainObjPtr &supervisor_domain, 
		const AosRundataPtr &rdata)
{
	if (!supervisor_acct || !self_domain || !supervisor_domain) return false;
	if (!self_domain->isFriendDomain(supervisor_domain)) return false;

	OmnString supervisors = getSupervisors();
	if (supervisors == "") return false;

	OmnString supervisor_objid = supervisor_acct->getObjid();
	if (supervisor_objid == "")
	{
		OmnAlarm << "Missing objid!" << enderr;
			return false;
	}

	return OmnStrParser1::containWord(supervisors, supervisor_objid);
}


bool
AosUserAcct::isSysAcct() const
{
	if (!mDoc) return false;
	if (mDoc->getAttrStr(AOSTAG_SYSACCT) != AOSSTYPE_SYSACCT) return false;
	return AosXmlUtil::verifySysAcct(mDoc);
}


bool
AosUserAcct::verifySysAcct(const AosXmlTagPtr &acct, const AosRundataPtr &rdata)
{
	// This function should be called by AosSite to verify 'acct' is indeed
	// a system account. This is because system accounts created before
	// this release (09/08/2011) did not have the needed flags.
	bool modified = false;
	if (acct->getAttrStr(AOSTAG_SYSACCT) != AOSSTYPE_SYSACCT)
	{
		acct->setAttr(AOSTAG_SYSACCT, AOSSTYPE_SYSACCT);
		modified = true;
	}
	
	if (!AosXmlUtil::verifySysAcct(acct))
	{
		OmnString signature = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
		acct->setAttr(AOSTAG_SYSACCT_SIGNATURE, signature);
		modified = true;
	}

	if (!modified) return true;
	
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	docclient->modifyObj(acct, rdata);
	aos_assert_r(AosXmlUtil::verifySysAcct(acct), false);
	return true;
}


AosXmlTagPtr
AosUserAcct::getUserOprArd(const AosRundataPtr &rdata)
{
	// User Operation Access Record is a doc.
	if (mUserOprArd) return mUserOprArd;
	aos_assert_rr(mDoc, rdata, 0);
	if (mUserOprArdTried) return 0;
	mUserOprArdTried = true;
	OmnString userid = mDoc->getAttrStr(AOSTAG_DOCID);
	aos_assert_rr(userid != "", rdata, 0);
	OmnString objid = AosObjid::composeUserOprArdObjid(userid);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	mUserOprArd = docclient->getDocByObjid(objid, rdata);
	if (!mUserOprArd) return 0;
	
	aos_assert_r(mUserOprArd->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USER_OPRARD, mUserOprArd);
	aos_assert_r(mUserOprArd->getAttrStr(AOSTAG_STYPE) == AOSSTYPE_USER_OPRARD, mUserOprArd);
	return mUserOprArd;
}


OmnString
AosUserAcct::getUsername()
{
	if (!mDoc) return "";
	return mDoc->getAttrStr(AOSTAG_USERNAME);
}




u64 
AosUserAcct::getDomainDocid(const u64 &userid, const AosRundataPtr &rdata)
{
	// This function retrieves the docid of the user domain to which the user 'userid'
	// belongs. 
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	aos_assert_r(userid, 0);
	AosXmlTagPtr userdoc = docclient->getDocByDocid(userid, rdata);
	aos_assert_r(userdoc, 0);
	OmnString domain_objid = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(domain_objid != "", 0);
	u64 domain_docid = docclient->getDocidByObjid(domain_objid, rdata);
	aos_assert_r(domain_docid, 0);
	return domain_docid;
}


OmnString 
AosUserAcct::getCloudid() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_CLOUDID);
}


OmnString 
AosUserAcct::getDomainObjid() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_PARENTC);
}


OmnString 
AosUserAcct::getObjid() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_OBJID);
}


OmnString 
AosUserAcct::getFriendDomains() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_FRIEND_DOMAINS);
}


OmnString 
AosUserAcct::getAttrStr(const OmnString &aname, const OmnString &dft)
{
	if (!mDoc) return dft;
	return mDoc->getAttrStr(aname, dft);
}


bool 
AosUserAcct::isUnknownUser() const 
{
	aos_assert_r(mDoc, true);
	return mDoc->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_UNKNOWN;
}


bool 
AosUserAcct::isGuestUser() const
{
	aos_assert_r(mDoc, false);
	return mDoc->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_GUEST;
}


OmnString 
AosUserAcct::getAttrStr(const OmnString &name, const OmnString &dft) const
{
	if (!mDoc) return dft;
	return mDoc->getAttrStr(name, dft);
}


OmnString 
AosUserAcct::getSupervisors() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_SUPERVISOR);
}

bool 
AosUserAcct::isSysRoot()
{
	if (!mDoc) return false;
	if (mDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_USERACCT) return false;
	if (mDoc->getAttrStr(AOSTAG_CLOUDID) == AOSCLOUDID_SYSROOT &&
		mDoc->getAttrStr(AOSTAG_CREATOR) == AOSCLOUDID_SYSROOT) return true;
	return false;
}


bool 
AosUserAcct::isSuperUser()
{
	if (!mDoc) return false;
	if (mDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_USERACCT) return false;
	if (mDoc->getAttrStr(AOSTAG_CLOUDID) == AOSCLOUDID_ROOT &&
		mDoc->getAttrStr(AOSTAG_CREATOR) == AOSCLOUDID_ROOT) return true;
	return false;
}


bool 
AosUserAcct::isRoot() const
{
	aos_assert_r(mDoc, false);
	return (mDoc->getAttrStr(AOSTAG_ACCT_TYPE) == AOSUSERTYPE_ROOT);
}


bool 
AosUserAcct::isAdmin() const
{
	aos_assert_r(mDoc, false);
	return mDoc->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_ROOT ||
	 	   mDoc->getAttrBool(AOSTAG_SYSADMIN);
}

/*
bool 
AosUserAcct::isAdmin(const AosXmlTagPtr &userdoc)
{
	if (!userdoc) return false;
	return userdoc->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_ROOT ||
	 	   userdoc->getAttrBool(AOSTAG_SYSADMIN);
}
*/

OmnString 
AosUserAcct::getUserDomains() const
{
	// All user domains are stored in the attribute AOSTAG_USERDOMAINS
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_USERDOMAINS);
}


OmnString 
AosUserAcct::getUserGroups() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_GROUPS);
}


OmnString 
AosUserAcct::getUserRoles() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_ROLES);
}


AosXmlTagPtr 
AosUserAcct::getDoc() const 
{
	return mDoc;
}


OmnString 
AosUserAcct::getHomeContainer() const
{
	aos_assert_r(mDoc, "");
	return mDoc->getAttrStr(AOSTAG_CTNR_HOME);
}


u64 
AosUserAcct::getDocid(const u64 &dft) const
{
	if (!mDoc) return dft;
	return mDoc->getAttrU64(AOSTAG_DOCID, dft);
}


bool 
AosUserAcct::isSameAccount(const AosUserAcctObjPtr &rhs)
{
	if (!rhs) return false;
	if (!mDoc) return false;
	AosXmlTagPtr doc = rhs->getDoc();
	if (!doc) return false;
	u64 docid = mDoc->getAttrU64(AOSTAG_DOCID, 0);
	if (!docid) return false;
	return docid == doc->getAttrU64(AOSTAG_DOCID, 0);
}


bool
AosUserAcct::getValue(AosXmlTagPtr &doc)
{
	doc = mDoc;
	return mUserOprArdTried;
}


void
AosUserAcct::setValue(const AosXmlTagPtr &doc, const bool &userOprArdTried)
{
	mUserOprArd = doc;
	mUserOprArdTried = userOprArdTried;
}
