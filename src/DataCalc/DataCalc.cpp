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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCalc/DataCalc.h"

static AosDataCalc dd("", "", "", "");

AosDataCalc::AosDataCalc(
		const OmnString &name, 
		const OmnString &libname, 
		const OmnString &method, 
		const OmnString &version)
:
AosDataCalcObj(name, libname, method, version)
{
}


AosDataCalc::~AosDataCalc()
{
}


bool 
AosDataCalc::run(const AosRundataPtr &rdata, 
		AosValueRslt &input,
		AosValueRslt &output)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCalc::run(const AosRundataPtr &rdata, 
		const char *field, 
		const int len,
		AosValueRslt &output)
{
	OmnShouldNeverComeHere;
	return false;
}
