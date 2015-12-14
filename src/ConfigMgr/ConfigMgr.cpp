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
#include "ConfigMgr/ConfigMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/ValueObj.h"


AosConfigMgr::AosConfigMgr()
{
	OmnNotImplementedYet;
}


AosConfigMgr::~AosConfigMgr()
{
}



OmnString 
AosConfigMgr::getItemStr(
		const AosRundataPtr &rdata, 
		const OmnString &config_name,
		const OmnString &name, 
		const OmnString &dft)
{
	OmnNotImplementedYet;
	return dft;
}



i64 
AosConfigMgr::getItemInt(
		const AosRundataPtr &rdata, 
		const OmnString &config_name,
		const OmnString &name, 
		const i64 dft)
{
	OmnNotImplementedYet;
	return dft;
}


vector<AosValueObjPtr> 
AosConfigMgr::getItems(
		const AosRundataPtr &rdata, 
		const OmnString &config_name,
		const OmnString &module_name)
{
	vector<AosValueObjPtr> vec;
	OmnNotImplementedYet;
	return vec;
}


bool 
AosConfigMgr::createConfig(
		const AosRundataPtr &rdata, 
		const OmnString &config_name)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosConfigMgr::copyConfig(
		const AosRundataPtr &rdata, 
		const OmnString &from_name, 
		const OmnString &to_name)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosConfigMgr::dropConfig(const AosRundataPtr &rdata, const OmnString &config_name)
{
	OmnNotImplementedYet;
	return false;
}

