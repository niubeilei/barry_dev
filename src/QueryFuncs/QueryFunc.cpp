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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryFuncs/QueryFunc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Mutex.h"


AosQueryFunc::AosQueryFunc(
		const OmnString &type,
		const int version)
:
AosQueryFuncObj(version),
mType(type)
{
}


AosQueryFunc::~AosQueryFunc()
{
}


bool
AosQueryFunc::pickJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	OmnShouldNeverComeHere;
	value.reset();
	return false;
}

