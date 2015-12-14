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
#include "SEInterfaces/WriteCacherObj.h"
#include "SEInterfaces/DataCacherType.h"

AosWriteCacherObj::AosWriteCacherObj(
		const OmnString &name, 
		const AosDataCacherType::E type, 
		const bool flag)
:
AosDataCacherObj(name, type, flag)
{
	//OmnScreen << "WriteCacherObj" << endl;
}

AosWriteCacherObj::AosWriteCacherObj()
{
}

AosWriteCacherObj::~AosWriteCacherObj()
{
}

/*
AosWriteCacherObjPtr AosWriteCacherObj::smWriteCacher;


void 
AosWriteCacherObj::setWriteCacher(const AosWriteCacherObjPtr &obj) 
{
	smWriteCacher = obj;
}


AosWriteCacherObjPtr 
AosWriteCacherObj::getWriteCacher(const AosWriteCacherObjPtr &obj) 
{
	return smWriteCacher;
}
*/

