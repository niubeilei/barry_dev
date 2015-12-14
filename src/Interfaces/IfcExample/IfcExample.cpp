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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Interfaces/IfcExample/IfcExample.h"

#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosIfcExample::AosIfcExample()
:
mMethod1(0),
mMethod2(0)
{
}


AosIfcExample::~AosIfcExample()
{
}


AosInterfacePtr 
AosIfcExample::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosIfcExample(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return 0;
	}

	return 0;
}


bool 
AosIfcExample::retrieveMethods(
		const AosRundataPtr &rdata, 
		const AosJimoPtr &jimo)
{
	resetMethods();
	bool rslt = AosInterface::retrieveMethods(rdata, jimo);
	aos_assert_rr(rslt, rdata, false);

	AosMethodId::E method_id;

	// Retrieve the first method
	void *method = getMethod(rdata, AOSMETHODNAME_BASIC001, method_id);
	if (method)
	{
		aos_assert_rr(method_id == AosMethodId::eBasic001, rdata, false);
		mMethod1 = (AosJimoBasicFunc001)method;
	}

	// Retrieve the second method
	method = getMethod(rdata, AOSMETHODNAME_BASIC002, method_id);
	if (method)
	{
		aos_assert_rr(method_id == AosMethodId::eBasic002, rdata, false);
		mMethod2 = (AosJimoBasicFunc002)method;
	}

	return true;
}


void
AosIfcExample::resetMethods()
{
	mMethod1 = 0;
	mMethod2 = 0;
}

