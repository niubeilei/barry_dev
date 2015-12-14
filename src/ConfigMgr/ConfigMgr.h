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
// 2014/01/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ConfigMgr_ConfigMgr_h
#define AOS_ConfigMgr_ConfigMgr_h

#include "SEInterfaces/ConfigMgrObj.h"


class AosConfigMgr :  public AosConfigMgrObj
{
	OmnDefineRCObject;

private:

public:
	AosConfigMgr();
	~AosConfigMgr();

	virtual OmnString getItemStr(
							const AosRundataPtr &rdata, 
							const OmnString &config_name,
							const OmnString &name, 
							const OmnString &dft);

	virtual i64 getItemInt(
							const AosRundataPtr &rdata, 
							const OmnString &config_name,
							const OmnString &name, 
							const i64 dft);

	virtual vector<AosValueObjPtr> getItems(
							const AosRundataPtr &rdata, 
							const OmnString &config_name,
							const OmnString &module_name);

	virtual bool createConfig(
							const AosRundataPtr &rdata, 
							const OmnString &config_name);

	virtual bool copyConfig(
							const AosRundataPtr &rdata, 
							const OmnString &from_name, 
							const OmnString &to_name);

	virtual bool dropConfig(const AosRundataPtr &rdata, 
							const OmnString &config_name);
};
#endif
