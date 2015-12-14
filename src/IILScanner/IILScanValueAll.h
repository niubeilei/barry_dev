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
// 2012/11/12 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILScanner_IILScanValueAll_h
#define Aos_IILScanner_IILScanValueAll_h

#include "IILScanner/IILScanValue.h"


class AosIILScanValueAll : public AosIILScanValue
{

public:
	AosIILScanValueAll(const bool regist_flag);
	~AosIILScanValueAll();

	virtual AosIILScanValuePtr clone(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);

	virtual bool scanValue(
					vector<OmnString> &keys,
					vector<u64> &values,
					vector<OmnString> &out_keys,
					vector<u64> &out_values,
					const AosRundataPtr &rdata);

private:
	virtual bool config(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);
};
#endif
