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
//
// Modification History:
//	06/06/2010	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEBase_SecReq_h
#define Omn_SEBase_SecReq_h

#include "Rundata/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "UserMgmt/Ptrs.h"
#include "SEBase/SecOpr.h"
#include "SEUtil/ObjidType.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/UserDomainObj.h"
#include "SEInterfaces/UserAcctObj.h"


class AosSecReq
{
private:
	OmnString			mOprStr;

	OmnMutexPtr			mLock;
	AosXmlTagPtr		mAccessedObj;
	AosXmlTagPtr		mAccessedParentCtnr;
	AosUserAcctObjPtr	mAccessedAcct;
	AosAccessRcdPtr		mAccessedAR;
	AosUserDomainObjPtr	mAccessedDomain;
	AosUserAcctObjPtr	mRequesterAcct;
	AosUserDomainObjPtr	mRequesterDomain;
	AosXmlTagPtr		mReservedObjidAccesses;

public:
	AosSecReq();
	AosSecReq(
			const OmnString &operation,
			const AosXmlTagPtr &acc_doc,
			const AosRundataPtr &rdata);

	// Chen Ding, 12/28/2012
	static bool config(const AosXmlTagPtr &config);

	// Chen Ding, 12/28/2012
	bool setData(
			const OmnString &operation,
			const AosXmlTagPtr &accessed_container,
			const AosXmlTagPtr &acc_doc,
			const AosRundataPtr &rdata);

	static u64 getAccessedDocUserid(
						const AosXmlTagPtr &accessed_doc, 
						const AosRundataPtr &rdata);
	inline u64 getAccessedDocUserid(const AosRundataPtr &rdata) const
	{
		return getAccessedDocUserid(mAccessedObj, rdata);
	}

	AosXmlTagPtr	getAccessedObj() const;
	AosXmlTagPtr	getAccessedParentCtnr(const AosRundataPtr &rdata);
	// AosAccessRcdPtr	getAccessedAR() const;

	// Chen Ding, 12/28/2012
	OmnString getOprStr() const 
	{
		return mOprStr;
	}

	bool 			doesRequesterOwnAccessed(const AosRundataPtr &rdata);
	AosXmlTagPtr	getAccesses(
						const OmnString &opr_str, 
						const AosRundataPtr &rdata);
	bool			isRoot(const AosRundataPtr &rdata);
	static bool		isRequesterAdmin(const AosRundataPtr &rdata);

	// Chen Ding, 06/06/2012
	bool isUserLoggedInPublic(const AosRundataPtr &rdata)
	{
		mLock->lock();
		bool rslt = isUserLoggedInLocked(rdata);
		mLock->unlock();
		return rslt;
	}
	bool			requireLogin(OmnString &hpvpd);
	OmnString		getDocOperationEvent() const;
	AosXmlTagPtr	getRequesterOprArd(const AosRundataPtr &rdata);
	AosXmlTagPtr	getAccessedDomainOprArd(const AosRundataPtr &rdata);
	OmnString		toString() const;

	OmnString		getRequesterUserGroups() const;
	OmnString		getRequesterUserRoles() const;
	OmnString		getRequesterDomains() const;
	OmnString		getRequesterUsername();
	AosUserAcctObjPtr  getAccessedAcct() const;
	AosUserAcctObjPtr  getRequesterAcct() const;
	AosXmlTagPtr	getAccessedCtnr(const AosRundataPtr &rdata) const;
	void			setReservedObjidAccesses(const AosXmlTagPtr &acc);

	// Chen Ding, 06/06/2012
	bool isAdminPublic(const AosRundataPtr &rdata)
	{
		mLock->lock();
		bool rslt = isAdminLocked(rdata);
		mLock->unlock();
		return rslt;
	}
	inline bool	isSameDomainPublic() const
	{
		mLock->lock();
		bool rslt = isSameDomainLocked();
		mLock->unlock();
		return rslt;
	}

private:
	bool initLocked(const AosRundataPtr &rdata);
	bool isAdminLocked(const AosRundataPtr &rdata);
	bool isUserLoggedInLocked(const AosRundataPtr &rdata);
	bool isSameDomainLocked() const;

public:
	// Chen Ding, 12/28/2012
	void reset();

	// Chen Ding, 12/28/2012
	bool checkReservedObjid(
				const OmnString &objid, 
				const AosXmlTagPtr &doc,
				const AosObjidType::E objid_type, 
				const AosRundataPtr &rdata);
	OmnString getAccessedObjid() const
	{
		if (!mAccessedObj) return "";
		return mAccessedObj->getAttrStr(AOSTAG_OBJID);
	}
};
#endif

