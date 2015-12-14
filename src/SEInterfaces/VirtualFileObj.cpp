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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEInterfaces/VirtualFileObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VirtualFileCreator.h"
#include "XmlUtil/XmlTag.h"


AosVirtualFileCreatorObjPtr AosVirtualFileObj::smCreator;

AosVirtualFileObj::AosVirtualFileObj(
		const OmnString &name, 
		const AosVirtualFileType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		AosVirtualFileObjPtr thisptr(this, false);
		if (!AosVirtualFileObj::registerVirtualFile(name, thisptr))
		{
			OmnThrowException("failed_registering");
			return;
		}
	}
}


AosVirtualFileObj::AosVirtualFileObj()
{
}


AosVirtualFileObj::~AosVirtualFileObj()
{
}


AosVirtualFileObjPtr 
AosVirtualFileObj::createVirtualFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createVirtualFile(def, rdata);
}
	

bool
AosVirtualFileObj::registerVirtualFile(
		const OmnString &name, 
		const AosVirtualFileObjPtr &virtual_file)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerVirtualFile(name, virtual_file);
}
#endif
