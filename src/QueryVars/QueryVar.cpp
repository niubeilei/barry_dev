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
#include "QueryVars/QueryVar.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Mutex.h"


AosQueryVar::AosQueryVar(
		const OmnString &type,
		const int version)
:
AosQueryVarObj(version),
mType(type)
{
}


AosQueryVar::~AosQueryVar()
{
}


bool 
AosQueryVar::pickJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosQueryVar::procDefault(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	// It supports three default value processing:
	// 	1. IGNORE: the query shall be ignored. The caller should set the term
	// 	   to be invalid.
	// 	2. USE DEFAULT: use the default value.
	// 	3. USE IT: just use the empty vallue.
	OmnString default_policy = def->getAttrStr("default_policy", AOSVALUE_AS_IS);
	if (default_policy == AOSVALUE_USEDEFAULT)
	{
		value.setStr(def->xpathQuery("default/_#text"));
		return true;
	}

	if (default_policy == AOSVALUE_IGNORE)
	{
		value.reset();
	}

	return true;
}

