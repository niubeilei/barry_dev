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
#include "GenericObjs/GenericMethod.h"


AosGenericMethod::AosGenericMethod(const int version)
:
AosGenericMethodObj(version)
{
}


AosGenericMethod::~AosGenericMethod()
{
}


bool
AosGenericMethod::proc(
	AosRundata *rdata, 
	const OmnString &obj_name, 
	const vector<AosExprObjPtr> &parms)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosGenericMethod::config(
	const AosRundataPtr &rdata,
	const AosXmlTagPtr &worker_doc,
	const AosXmlTagPtr &jimo_doc)
{
	OmnShouldNeverComeHere;
	return NULL;
}


AosJimoPtr 
AosGenericMethod::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return NULL;
}

