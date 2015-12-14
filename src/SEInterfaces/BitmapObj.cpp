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
// 11/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapObj.h"

AosBitmapObjPtr	AosBitmapObj::smObject;

AosBitmapObj::~AosBitmapObj()
{
}


AosBitmapObjPtr 
AosBitmapObj::createBitmapStatic()
{
	aos_assert_r(smObject, 0);
	return smObject->createBitmap();
}


bool 
AosBitmapObj::setObject(const AosBitmapObjPtr &obj)
{
	smObject = obj;
	return true;
}


AosBitmapObjPtr
AosBitmapObj::getObject()
{
	return smObject;
}

	
// void
// AosBitmapObj::countRsltAndStatic(
// 		const AosBitmapObjPtr &a,
// 		const AosBitmapObjPtr &b)
//{
// 	aos_assert(smObject);
// 	smObject->countRsltAnd(a, b);
//}


//void
//AosBitmapObj::countRsltOrStatic(
// 		const AosBitmapObjPtr &a,
// 		const AosBitmapObjPtr &b)
//{
// 	aos_assert(smObject);
// 	return smObject->countRsltOr(a, b);
//}

