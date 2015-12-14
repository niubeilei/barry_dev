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
// 09/01/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UserMgmt_UserAcctMgr_h
#define Aos_UserMgmt_UserAcctMgr_h

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

OmnDefineSingletonClass(AosUserAcctMgrSingleton,
						AosUserAcctMgr,
						AosUserAcctMgrSelf,
						OmnSingletonObjId::eUserAcctMgr,
						"UserAcctMgr");

class AosUserAcctMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosUserAcctMgr();
	~AosUserAcctMgr();

	// Singleton Class Interface
	static AosUserAcctMgr*    getSelf();
	virtual bool	start();
	virtual bool 	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	AosUserAcctObjPtr getUserAcct(
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata);

	virtual AosUserAcctObjPtr getUserAcctByCloudid(
			const OmnString &cid, 
			const AosRundataPtr &rdata);

	virtual AosUserAcctObjPtr getUserAcctByDocid(
			const u64 &docid, 
			const AosRundataPtr &rdata);

	AosUserAcctObjPtr getUserAcctByObjid(
			const OmnString &objid, 
			const AosRundataPtr &rdata);

	// Chen Ding, 12/30/2012
	virtual bool checkManagership(
					const AosUserAcctObjPtr &requester,
					const AosUserAcctObjPtr &user,
					const OmnString &manager_attrname,
					const bool direct_mgr,
					const AosRundataPtr &rdata);

private:
};
#endif

