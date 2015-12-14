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
// 2015/03/07 Created by Arvin 
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/Jimos/StatTime.h"


AosStatTime::AosStatTime()
{
}

AosStatTime::~AosStatTime()
{
}


bool
AosStatTime::config(const AosXmlTagPtr &def)
{
	mTimeFieldName = def->getAttrStr("time_field_name","");
	mGrpbyTimeUnit  = def->getAttrStr("grpby_time_unit", "");
	mTimeFormat   = def->getAttrStr("time_format","");
	return true;
}

/*
OmnString
AosStatTime::toXmlString()
{
	OmnString str("<time_field ");
	str << AOSTAG_TIMEUNIT << "=\"" << mGrpbyTimeUnit << "\" "
		<< AOSTAG_TIMEFORMAT << "=\"" << mTimeFormat << "\" "
		<< "field_name" << "=\"" << mFieldName << "\" "
		<< "field_type" << "=\"" << AosStatField::typeEnumToStr(mFieldType) << "\" "
		<< "zky_data_type" << "=\"" << AosDataType::getTypeStr(mDataType) << "\" "
        << "></time_field>";

	return str;
}
*/

