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
// 09/05/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_SystemDocMgr_h
#define Aos_XmlUtil_SystemDocMgr_h

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

OmnDefineSingletonClass(AosSystemDocMgrSingleton,
						AosSystemDocMgr,
						AosSystemDocMgrSelf,
						OmnSingletonObjId::eSystemDocMgr,
						"SystemDocMgr");

class AosSystemDocMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosSystemDocMgr();
	~AosSystemDocMgr();

	// Singleton Class Interface
	static AosSystemDocMgr*    getSelf();
	virtual bool	start();
	virtual bool 	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	AosSystemDocPtr getSystemDoc(
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata);
};
#endif

