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
// 10/19/2014 Created by Shawn Li
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/GroupQueryObj.h"

AosGroupQueryObjPtr	AosGroupQueryObj::smObject;

AosGroupQueryObj::~AosGroupQueryObj()
{
}


AosGroupQueryObjPtr 
AosGroupQueryObj::createQueryStatic(const AosRundataPtr &rdata)
{
	aos_assert_r(smObject, 0);
	return smObject->createQuery(rdata);
}


bool 
AosGroupQueryObj::setObject(const AosGroupQueryObjPtr &obj)
{
	smObject = obj;
	return true;
}


AosGroupQueryObjPtr
AosGroupQueryObj::getObject()
{
	return smObject;
}
