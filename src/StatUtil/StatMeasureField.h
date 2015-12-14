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
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_StatMeasureField_h
#define AOS_StatUtil_StatMeasureField_h

#include "StatUtil/StatField.h"

class AosStatMeasureField: public AosStatField
{

public:
	OmnString	mAggrFuncStr;
	OmnString	mRawFname;

	AosStatMeasureField();
	AosStatMeasureField(
			const OmnString &fname,
			const OmnString &raw_fname,
			const OmnString &aggr_func_str);
	
	virtual bool config(const AosXmlTagPtr &conf);
	
	virtual OmnString toXmlString();

};

#endif
