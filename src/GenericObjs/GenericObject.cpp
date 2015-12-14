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
//
// Modification History:
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/GenericObject.h"

#include "GenericObjs/Ptrs.h"
#include "GenericObjs/GenericMethod.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericMethodObj.h"


AosGenericObject::AosGenericObject(const int version)
:
AosGenericObj(version)
{
}


AosGenericObject::~AosGenericObject()
{
}


bool
AosGenericObject::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


