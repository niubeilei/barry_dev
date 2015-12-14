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
// 06/15/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_SeConfigMgr_h
#define AOS_SEUtil_SeConfigMgr_h

#include "Thread/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"

OmnDefineSingletonClass(AosSeConfigMgrSingleton,
	AosSeConfigMgr,
	AosSeConfigMgrSelf,
	OmnSingletonObjId::eSeConfigMgr,
	"SeConfigMgr");

extern AosXmlTagPtr gAosAppConfig;

class AosSeConfigMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString	mDftHomeCtnr;
	OmnString 	mNoOwnerGrpFlag;
	OmnString 	mNoOwnerRoleFlag;

public:
	AosSeConfigMgr();
	~AosSeConfigMgr();

	// Singleton class interface
	static AosSeConfigMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);

	OmnString	getDftHomeContainer() const {return mDftHomeCtnr;}
	OmnString	getNoOwnerGrpFlag() const {return mNoOwnerGrpFlag;}
	OmnString	getNoOwnerRoleFlag() const {return mNoOwnerRoleFlag;}

private:
};

#endif
