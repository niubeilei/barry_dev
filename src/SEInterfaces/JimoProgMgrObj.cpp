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
// Modification History:
// 2015/04/01 Created by Xia Fan
////////////////////////////////////////////////////////////////////////////

#include "SEInterfaces/JimoProgMgrObj.h"

AosJimoProgMgrObj::AosJimoProgMgrObj(const int version)
:
AosJimo(AosJimoType::eJimoProgMgr,version)
{
	mJimoType = AosJimoType::eJimoProgMgr;
}


