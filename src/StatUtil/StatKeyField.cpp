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

#include "StatUtil/StatKeyField.h"

AosStatKeyField::AosStatKeyField(const OmnString &fname)
:
AosStatField(eKeyField)
{
	setDataType(AosDataType::eString);	// Temp.
	setFieldName(fname);
}

AosStatKeyField::AosStatKeyField(const OmnString &fname, const AosDataType::E &fieldType)
:
AosStatField(eKeyField)
{
	setDataType(fieldType);	// Temp.
	setFieldName(fname);
}

AosStatKeyField::AosStatKeyField()
:
AosStatField(eKeyField)
{
	setDataType(AosDataType::eString);	// Temp.
}
	
bool
AosStatKeyField::config(const AosXmlTagPtr &conf)
{
	bool rslt = AosStatField::config(conf);	
	aos_assert_r(rslt, false);
	
	return true;	
}


OmnString
AosStatKeyField::toXmlString()
{
	OmnString str = "<field ";
	bool rslt = AosStatField::toXmlString(str);
	aos_assert_r(rslt, "");

	str << "></field>";

	return str;
}
	
