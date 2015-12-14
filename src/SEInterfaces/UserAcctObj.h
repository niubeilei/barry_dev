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
// 2015/01/08 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_UserAcctObj_h
#define AOS_SEInterfaces_UserAcctObj_h

#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"


class AosUserAcctObj : virtual public OmnRCObject
{

public:
	virtual OmnString getCloudid() const = 0;
	virtual	OmnString getDomainObjid() const = 0;
	virtual	OmnString getObjid() const = 0;
	virtual	OmnString getFriendDomains() const = 0;
	virtual	OmnString getAttrStr(const OmnString &aname, const OmnString &dft = "") = 0;
	virtual	bool isUnknownUser() const = 0;
	virtual	bool isGuestUser() const = 0;
	virtual	bool isSupervisor(
			const AosUserAcctObjPtr &supervisor_acct, 
			const AosUserDomainObjPtr &self_domain, 
			const AosUserDomainObjPtr &supervisor_domain, 
			const AosRundataPtr &rdata) = 0;
	virtual OmnString getAttrStr(const OmnString &name, const OmnString &dft = "") const = 0;
	virtual OmnString getSupervisors() const = 0;
	virtual bool isRoot(
			const AosUserDomainObjPtr &accessed_domain,
			const AosUserDomainObjPtr &requester_domain) const = 0;

	virtual bool isSysRoot() = 0;
	virtual bool isSuperUser() = 0;
	virtual bool isRoot() const = 0;
	virtual bool isAdmin(
			const AosUserDomainObjPtr &accessed_domain,
			const AosUserDomainObjPtr &requester_domain) const = 0;
	virtual bool isAdmin() const = 0;
	virtual OmnString getUserDomains() const = 0;
	virtual	OmnString getUserGroups() const = 0;
	virtual	OmnString getUserRoles() const = 0;
	virtual	AosXmlTagPtr getDoc() const = 0;
	virtual	OmnString getHomeContainer() const = 0;
	virtual	u64 getDocid(const u64 &dft) const = 0;
	virtual	bool isSameAccount(const AosUserAcctObjPtr &rhs) = 0;
	virtual	bool isSysAcct() const = 0;
	virtual	AosXmlTagPtr getUserOprArd(const AosRundataPtr &rdata) = 0;
	virtual	OmnString getUsername() = 0;
	static bool isAdmin(const AosXmlTagPtr &userdoc)
	{
		if (!userdoc) return false;
		return userdoc->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_ROOT ||
	 		   userdoc->getAttrBool(AOSTAG_SYSADMIN);
	}
	static bool isUserAcct(const AosXmlTagPtr &doc)
	{
		if (!doc) return false;
		return doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT;
	}
	static bool verifySysAcct(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	static OmnString getUsername(const u64 &userid, const AosRundataPtr &rdata);
	static AosXmlTagPtr getDomainDoc(const u64 &userid, const AosRundataPtr &rdata);
};
#endif
