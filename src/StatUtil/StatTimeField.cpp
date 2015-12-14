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
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatTimeField.h"


AosStatTimeField::AosStatTimeField()
:
AosStatField(eTimeField),
mTimeUnit(AosStatTimeUnit::eInvalid)
{
	setDataType(AosDataType::eInt64);	// Temp.
}


AosStatTimeField::AosStatTimeField(
		AosStatTimeUnit::E time_unit,
		const OmnString &fname)
:
AosStatField(eTimeField),
mTimeUnit(time_unit)
{
	setDataType(AosDataType::eInt64);	// Temp.
	setFieldName(fname);
}


bool
AosStatTimeField::config(const AosXmlTagPtr &conf)
{
	bool rslt = AosStatField::config(conf);	
	aos_assert_r(rslt, false);
	
	OmnString time_unit_str = conf->getAttrStr("time_unit", "");
	//aos_assert_r(time_unit_str != "", false);
	if(time_unit_str == "")	return true;	// temp. when cond contain time_cond. not have time_unit.

	mTimeUnit = AosStatTimeUnit::getTimeUnit(time_unit_str); 
	return true;
}


OmnString
AosStatTimeField::toXmlString()
{
	OmnString str = "<field ";
	bool rslt = AosStatField::toXmlString(str);
	aos_assert_r(rslt, "");

	if(mTimeUnit != AosStatTimeUnit::eInvalid)	// temp. when cond contain time_cond. not have time_unit.
	{
		str << "time_unit=\"" << AosStatTimeUnit::toStr(mTimeUnit) << "\" ";
	}

	str << "></field>";

	return str;
}


