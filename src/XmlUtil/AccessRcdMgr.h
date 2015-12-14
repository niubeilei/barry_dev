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
#ifndef Aos_XmlUtil_AccessRcdMgr_h
#define Aos_XmlUtil_AccessRcdMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/AccessRcd.h"
#include <map>
using namespace std;

OmnDefineSingletonClass(AosAccessRcdMgrSingleton,
						AosAccessRcdMgr,
						AosAccessRcdMgrSelf,
						OmnSingletonObjId::eAccessRcdMgr,
						"AccessRcdMgr");

class AosAccessRcdMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosAccessRcdMgr();
	~AosAccessRcdMgr();

	// Singleton Class Interface
	static AosAccessRcdMgr*    getSelf();
	virtual bool	start();
	virtual bool 	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	AosAccessRcdPtr convertToAccessRecord(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc);

private:
};
#endif

