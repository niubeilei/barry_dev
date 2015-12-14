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
#include "SEInterfaces/ReadCacherObj.h"

AosReadCacherObjPtr AosReadCacherObj::smReadCacher;


void 
AosReadCacherObj::setReadCacher(const AosReadCacherObjPtr &obj) 
{
	smReadCacher = obj;
}


AosReadCacherObjPtr 
AosReadCacherObj::getReadCacher(const AosReadCacherObjPtr &obj) 
{
	return smReadCacher;
}

