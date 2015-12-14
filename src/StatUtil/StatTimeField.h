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
#ifndef AOS_StatUtil_StatTimeField_h
#define AOS_StatUtil_StatTimeField_h

#include "StatUtil/StatField.h"
#include "StatUtil/StatTimeUnit.h"


class AosStatTimeField: public AosStatField
{

public:
	AosStatTimeUnit::E	mTimeUnit;

public:
	AosStatTimeField();
	AosStatTimeField(
			AosStatTimeUnit::E time_unit,
			const OmnString &fname);

	virtual	AosStatTimeUnit::E getTimeUnit(){return mTimeUnit;}

	virtual bool config(const AosXmlTagPtr &conf);
	
	virtual OmnString toXmlString();

};

#endif
