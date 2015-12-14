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
// 01/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
// Ketty 2013/05/09

#include "SEInterfaces/CubeTransSvrObj.h"

AosCubeTransSvrObjPtr AosCubeTransSvrObj::smTransSvr;
	

void
AosCubeTransSvrObj::setTransSvr(const AosCubeTransSvrObjPtr &d)
{
	smTransSvr = d;
	AosTransSvrObj::setTransSvr(d.getPtr());
}
