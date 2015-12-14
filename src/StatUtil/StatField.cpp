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

#include "StatUtil/StatField.h"

#include "StatUtil/StatKeyField.h"
#include "StatUtil/StatTimeField.h"
#include "StatUtil/StatMeasureField.h"
	
AosStatFieldPtr
AosStatField::configStatic(const AosXmlTagPtr &conf)
{
	AosStatFieldPtr field;

	OmnString tp_str = conf->getAttrStr("field_type", "");
	Type tp = typeStrToEnum(tp_str);	
	aos_assert_r(tp != eInvalid, 0);
	switch(tp)
	{
	case eKeyField:
		field = OmnNew AosStatKeyField();
		break;

	case eTimeField:
		field = OmnNew AosStatTimeField();
		break;

	case eMeasureField:
		field = OmnNew AosStatMeasureField();
		break;

	default:
		break;
	}
	
	aos_assert_r(field, 0);
	bool rslt = field->config(conf);
	aos_assert_r(rslt, 0);
	return field;
}


bool
AosStatField::config(const AosXmlTagPtr &conf)
{
	mName = conf->getAttrStr("zky_name", "");
	aos_assert_r(mName != "", false);
	
	OmnString tp_str = conf->getAttrStr("field_type", "");
	mFieldType = typeStrToEnum(tp_str);	
	aos_assert_r(mFieldType != eInvalid, false);
	
	OmnString data_tp_str = conf->getAttrStr("data_type", "");
	mDataType = AosDataType::toEnum(data_tp_str);	
	
	//arvin 2015.07.29
	OmnString type_str = conf->getAttrStr("type","");
	mType = AosDataType::toEnum(type_str);
	
	if (mDataType == AosDataType::eNumber)
	{
		OmnString str;
		u32 dft = 0;

		str = conf->getAttrStr("zky_data_total_digits", "18");
		mNumber.setTotalDigits(str.parseU32(dft));

		str = conf->getAttrStr("zky_data_small_digits", "2");
		mNumber.setSmallDigits(str.toInt());
	}

	return true;
}

bool
AosStatField::toXmlString(OmnString &str)
{
	str << "zky_name=\"" << mName << "\" "
		<< "field_type=\"" << typeEnumToStr(mFieldType) << "\" ";
	if(mDataType != AosDataType::eInvalid)
	{
		str << "data_type=\"" << AosDataType::getTypeStr(mDataType) << "\" ";
		//arvin 2015.07.29
		str << "type=\"" << AosDataType::getTypeStr(mType) << "\" ";

		if (mDataType == AosDataType::eNumber)
		{
			str << "zky_data_total_digits=\"" << mNumber.getTotalDigits() << "\" "
				<< "zky_data_small_digits=\"" << mNumber.getSmallDigits() << "\" ";
		}
	}
	return true;
}

AosStatField::Type 
AosStatField::typeStrToEnum(const OmnString str)
{
	if(str == "key_field")	return eKeyField;	
	if(str == "time_field")	return eTimeField;	
	if(str == "measure_field")	return eMeasureField;	
	return eInvalid;
}


OmnString
AosStatField::typeEnumToStr(const Type tp)
{
	switch(tp)
	{
	case eKeyField:
		return "key_field";
	
	case eTimeField:
		return "time_field";

	case eMeasureField:
		return "measure_field";

	default:
		break;
	}
	return "";
}

