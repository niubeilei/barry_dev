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
// 03/26/2008: Copied from Util/Object.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Obj/Object.h"



u64 AosObject::mTotalActiveObjects = 0;
u64	AosObject::mTotalObjects = 0;



AosObject::AosObject()
:
// AosInitObjPreguard(__AosObject, AOS_OBJECT_MAGIC)
mLineno(0)
// AosInitObjPostguard(__AosObject, AOS_OBJECT_MAGIC)
{
}


AosObject::~AosObject()
{
}


int 
AosObject::integrityCheck() const
{
	return 1;
}



