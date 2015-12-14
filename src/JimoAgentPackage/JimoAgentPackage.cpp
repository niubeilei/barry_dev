//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "JimoAgentPackage/JimoAgentPackage.h"
#include "JimoAgentPackage/JimoAgentCommand.h"
#include "JimoAgentPackage/JimoAgentCommandResp.h"
#include "JimoAgentPackage/JimoAgentFileToRemote.h"
#include "JimoAgentPackage/JimoAgentFileToRemoteResp.h"
#include "JimoAgentPackage/JimoAgentFileToLocal.h"
#include "JimoAgentPackage/JimoAgentFileToLocalResp.h"
#include "Thread/Mutex.h"

AosJimoAgentPackage::AosJimoAgentPackage(
		const AosAgentPackageType::E type,
		const OmnString &name)
:
mType(type),
mName(name)
{
}

bool
AosJimoAgentPackage::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	buff->setOmnStr(mName);
	return true;
}

bool 
AosJimoAgentPackage::serializeFrom(const AosBuffPtr &buff)
{
	return true;
}

AosPackagePtr 
AosJimoAgentPackage::serializeFromStatic(const AosBuffPtr &buff)
{
	aos_assert_r(buff, 0);
	AosAgentPackageType::E type = AosAgentPackageType::toEnum(buff->getOmnStr(""));
	if (!AosAgentPackageType::isValid(type))
	{
		return 0;
	}
	AosPackagePtr package = getPackage(type);
	if (!package)
	{
		return 0;
	}
	bool rslt = package->serializeFrom(buff);
	aos_assert_r(rslt, 0);
	return package;
}

AosPackagePtr
AosJimoAgentPackage::getPackage(const AosAgentPackageType::E type)
{
	switch(type)
	{
	case AosAgentPackageType::eCommand:
		return OmnNew AosJimoAgentCommand();
	case AosAgentPackageType::eCommandResp:
		return OmnNew AosJimoAgentCommandResp();
	case AosAgentPackageType::eFileToRemote:
		return OmnNew AosJimoAgentFileToRemote();
	case AosAgentPackageType::eFileToRemoteResp:
		return OmnNew AosJimoAgentFileToRemoteResp();
	case AosAgentPackageType::eFileToLocal:
		return OmnNew AosJimoAgentFileToLocal();
	case AosAgentPackageType::eFileToLocalResp:
		return OmnNew AosJimoAgentFileToLocalResp();
	default:
		return 0;
	}
}
