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
#ifndef Aos_UserMgmt_UserDomain_h
#define Aos_UserMgmt_UserDomain_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/UserAcctObj.h"
#include "SEInterfaces/UserDomainObj.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "UserMgmt/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosUserDomain : virtual public AosUserDomainObj
{
	OmnDefineRCObject;

private:
	AosUserAcctObjPtr	mUserAcct;
	bool				mSupervisorFlag;
	bool				mIsSysDomain;
	OmnString			mCtnrObjid;
	AosXmlTagPtr		mDomainDoc;

public:
    AosUserDomain(
			const OmnString &objid, 
			const AosUserAcctObjPtr &use_acct, 
			const AosRundataPtr &rdata);
    ~AosUserDomain();

	bool isGood() const {return (mUserAcct)?true:false;}
	bool isFriendDomain(const AosUserDomainObjPtr &domain);
	// OmnString getContainer() const {return mCtnrObjid;}
	OmnString getDomainObjid() const {return mCtnrObjid;}
	AosXmlTagPtr getDomainDoc(const AosRundataPtr &rdata);
	OmnString getFriendDomains() const
	{
		if (!mUserAcct) return "";
		return mUserAcct->getFriendDomains();
	}
	bool supervisorEnabled() const {return mSupervisorFlag;}
	bool isSysDomain() const {return mIsSysDomain;}
	AosXmlTagPtr getUserDomainOprArd(const AosRundataPtr &rdata);


private:
};
#endif

