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
// 2013/01/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/VfsMgrObj.h"

#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Util/File.h"

AosVfsMgrObjPtr AosVfsMgrObj::smVfsMgr;
vector<int>     AosVfsMgrObj::smDeviceId;

void
AosVfsMgrObj::configStatic(const AosXmlTagPtr &app_conf)
{
	aos_assert(app_conf);
	AosXmlTagPtr stm_cfg = app_conf->getFirstChild(AOSCONFIG_STORAGEMGR);
	aos_assert(stm_cfg);


	AosXmlTagPtr dev_config = stm_cfg->getFirstChild(AOSCONFIG_DEVICE);
	aos_assert(dev_config);

	AosXmlTagPtr partition_config = dev_config->getFirstChild(AOSCONFIG_PARTITION);
	while(partition_config)
	{
		OmnString userdir = partition_config->getAttrStr("userdir", "");
		if(userdir == "" ) 
		{
			partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
			continue;
		}

		u32 len = userdir.length();
		if(userdir.data()[len-1] != '/')   userdir << "/";

		int devid = OmnFile::getDeviceId(userdir);
		smDeviceId.push_back(devid);
		partition_config = dev_config->getNextChild(AOSCONFIG_PARTITION);
	}
	aos_assert(smDeviceId.size());
}


bool
AosVfsMgrObj::getDeviceId(vector<int> &devid)
{
	aos_assert_r(smDeviceId.size() != 0, false);
	devid = smDeviceId;
	return true;
}
