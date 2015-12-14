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
// 08/27/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UserMgmt_UserDomainMgr_h
#define Aos_UserMgmt_UserDomainMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEInterfaces/UserAcctObj.h"
#include <map>
using namespace std;

OmnDefineSingletonClass(AosUserDomainMgrSingleton,
						AosUserDomainMgr,
						AosUserDomainMgrSelf,
						OmnSingletonObjId::eUserDomainMgr,
						"UserDomainMgr");

class AosUserDomainMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr							mLock;
	map<OmnString, AosUserDomainObjPtr>	mUserDomains;

public:
	AosUserDomainMgr();
	~AosUserDomainMgr();

	// Singleton Class Interface
	static AosUserDomainMgr*    getSelf();
	virtual bool	start();
	virtual bool 	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	AosUserDomainObjPtr getUserDomain1(
					const OmnString &objid, 
					const AosUserAcctObjPtr &user_acct, 
					const AosRundataPtr &rdata);

	bool isFriendUserDomain(
					const AosUserDomainObjPtr &domain1, 
					const AosUserDomainObjPtr &domain2,
					const AosRundataPtr &rdata);

private:
};
#endif

