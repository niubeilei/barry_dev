////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2012/11/12 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILScanner/IILScanValueAll.h"


AosIILScanValueAll::AosIILScanValueAll(const bool regist_flag)
:
AosIILScanValue(AosIILScanValue::eAll, regist_flag)
{
}


AosIILScanValueAll::~AosIILScanValueAll()
{
}


bool
AosIILScanValueAll::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	return true;
}


AosIILScanValuePtr
AosIILScanValueAll::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosIILScanValueAll * scan = OmnNew AosIILScanValueAll(false);
	bool rslt = scan->config(def, rdata);
	aos_assert_r(rslt, 0);
	return scan;
}


bool
AosIILScanValueAll::scanValue(
		vector<OmnString> &keys,
		vector<u64> &values,
		vector<OmnString> &out_keys,
		vector<u64> &out_values,
		const AosRundataPtr &rdata)
{
	return true;
}

