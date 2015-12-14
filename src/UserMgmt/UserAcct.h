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
#ifndef AOS_UserMgmt_UserAcct_h
#define AOS_UserMgmt_UserAcct_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/StrParser.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/UserAcctObj.h"


class AosUserAcct : virtual public AosUserAcctObj 
{
	OmnDefineRCObject;

private:
	AosXmlTagPtr	mDoc;
	AosXmlTagPtr	mUserOprArd;
	bool			mUserOprArdTried;

public:
	AosUserAcct(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	~AosUserAcct() {}

	OmnString getCloudid() const;
	OmnString getDomainObjid() const;
	OmnString getObjid() const;
	OmnString getFriendDomains() const;
	OmnString getAttrStr(const OmnString &aname, const OmnString &dft = "");
	bool isUnknownUser() const;
	bool isGuestUser() const;
	bool isSupervisor(
			const AosUserAcctObjPtr &supervisor_acct, 
			const AosUserDomainObjPtr &self_domain, 
			const AosUserDomainObjPtr &supervisor_domain, 
			const AosRundataPtr &rdata);
	OmnString getAttrStr(const OmnString &name, const OmnString &dft = "") const;
	OmnString getSupervisors() const;
	bool isRoot(
			const AosUserDomainObjPtr &accessed_domain,
			const AosUserDomainObjPtr &requester_domain) const;

	bool isSysRoot();
	bool isSuperUser();
	bool isRoot() const;
	bool isAdmin(
			const AosUserDomainObjPtr &accessed_domain,
			const AosUserDomainObjPtr &requester_domain) const;
	bool isAdmin() const;
	//static bool isAdmin(const AosXmlTagPtr &userdoc);
	OmnString getUserDomains() const;
	OmnString getUserGroups() const;
	OmnString getUserRoles() const;
	AosXmlTagPtr getDoc() const;
	OmnString getHomeContainer() const;
	u64 getDocid(const u64 &dft) const;
	bool isSameAccount(const AosUserAcctObjPtr &rhs);
	bool isSysAcct() const;

	//static bool isUserAcct(const AosXmlTagPtr &doc);
	static bool verifySysAcct(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	static u64 getDomainDocid(const u64 &userid, const AosRundataPtr &rdata);

	AosXmlTagPtr getUserOprArd(const AosRundataPtr &rdata);
	OmnString getUsername();
	bool getValue(AosXmlTagPtr &doc);
	void setValue(const AosXmlTagPtr &doc, const bool &userOprArdTried);
};
#endif

