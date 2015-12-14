////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SingletonMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SingletonClass_SingletonMgr_h
#define Omn_SingletonClass_SingletonMgr_h

#include "Debug/Rslt.h"
#include "SingletonClass/SingletonObjId.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class OmnSingletonObj;

typedef OmnVList<OmnSingletonObj*>		OmnSingletonObjList;

class OmnSingletonMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnSingletonObjList			mSingletons;
	OmnMutexPtr					mLock;

public:
	OmnSingletonMgr();
	~OmnSingletonMgr();

	OmnRslt			addSingleton(OmnSingletonObj *creator);

	OmnRslt			start(const AosXmlTagPtr &conf);
	OmnRslt			createSingleton();
	OmnRslt			deleteSingleton();	
	OmnRslt			startSingleton();
	OmnRslt			stopSingleton();
	OmnRslt			configSingleton(const AosXmlTagPtr &conf);
	OmnRslt			startSingleton(const OmnSingletonObjId::E objId,
								   const AosXmlTagPtr &conf);
};

#endif
