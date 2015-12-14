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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/SecurityMgrObj.h"

AosSecurityMgrObjPtr AosSecurityMgrObj::smSecurityMgr;

AosSecurityMgrObjPtr 
AosSecurityMgrObj::getSecurityMgr() 
{
	return smSecurityMgr;
}

void 
AosSecurityMgrObj::setSecurityMgr(const AosSecurityMgrObjPtr &d) 
{
	smSecurityMgr = d;
}


