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
// 2013/05/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapMgrObj.h"


AosBitmapMgrObjPtr	AosBitmapMgrObj::smObject;

AosBitmapMgrObjPtr 
AosBitmapMgrObj::getBitmapMgr()
{
	aos_assert_r(smObject, 0);
	return smObject;
}


bool 
AosBitmapMgrObj::setBitmapMgr(const AosBitmapMgrObjPtr &obj)
{
	smObject = obj;
	return true;
}


AosBitmapObjPtr
AosBitmapMgrObj::getBitmapStatic()
{
	aos_assert_r(smObject, 0);
	return smObject->getBitmap();
}


void
AosBitmapMgrObj::returnBitmapStatic(const AosBitmapObjPtr &bitmap)
{
	aos_assert(smObject);
	smObject->returnBitmap(bitmap);
}

