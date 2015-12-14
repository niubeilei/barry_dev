////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/12/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ConfigMgrObj_h
#define Aos_SEInterfaces_ConfigMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosConfigMgrObj : virtual public OmnRCObject
{
private:
	static AosConfigMgrObjPtr	smConfigMgr;

public:
	// virtual bool registerHandler(
	// 					const AosRundataPtr &rdata, 
	// 					const AosConfigHandlerObjPtr &handler);
	virtual OmnString getItemStr(
							const AosRundataPtr &rdata, 
							const OmnString &config_name,
							const OmnString &name, 
							const OmnString &dft) = 0;

	virtual i64 getItemInt(
							const AosRundataPtr &rdata, 
							const OmnString &config_name,
							const OmnString &name, 
							const i64 dft) = 0;

	virtual vector<AosValueObjPtr> getItems(
							const AosRundataPtr &rdata, 
							const OmnString &config_name,
							const OmnString &module_name) = 0;

	virtual bool createConfig(
							const AosRundataPtr &rdata, 
							const OmnString &config_name) = 0;

	virtual bool dropConfig(const AosRundataPtr &rdata, 
							const OmnString &config_name) = 0;

	static AosConfigMgrObjPtr getConfigMgr() 
	{
		return smConfigMgr;
	}

	static void setConfigMgr(const AosConfigMgrObjPtr &conf)
	{
		smConfigMgr = conf;
	}
};
#endif
