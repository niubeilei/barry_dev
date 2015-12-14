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
// 07/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/StrValueInfo.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

bool 
AosStrValueInfo::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	field_offset = def->getAttrInt(AOSTAG_OFFSET, -1);
	field_data_len = def->getAttrInt(AOSTAG_LENGTH, -1);
	//set_field_null = def->getAttrBool(AOSTAG_SET_FIELD_NULL, false);
	//field_len = field_data_len;
	//if (set_field_null) field_len++;

	// Retrieve 'data-too-long-policy'
	OmnString policy = def->getAttrStr(AOSTAG_TOO_LONG_POLICY);
	data_too_long_policy = AosDataTooLongPolicy::toEnum(policy);

	// Retrieve 'data-too-short-policy'
	policy = def->getAttrStr(AOSTAG_TOO_SHORT_POLICY);
	data_too_short_policy = AosDataTooShortPolicy::toEnum(policy);

	// Retrieve 'data-null-policy'
	policy = def->getAttrStr(AOSTAG_NULL_POLICY);
	data_null_policy = AosDataNullPolicy::toEnum(policy);

	front_padding = def->getAttrChar(AOSTAG_FRONT_PADDING, ' ');
	back_padding = def->getAttrChar(AOSTAG_BACK_PADDING, ' ');
	null_padding = def->getAttrChar(AOSTAG_NULL_PADDING, ' ');
	too_short_padding = def->getAttrChar(AOSTAG_TOO_SHORT_PADDING, ' ');
	too_long_padding = def->getAttrChar(AOSTAG_TOO_LONG_PADDING, ' ');

	set_trailing_null = def->getAttrBool(AOSTAG_SET_TRAILING_NULL, false);
	notnull = def->getAttrBool("zky_notnull", false);

	// Chen Ding, 2014/12/04
	
	// ANDY
#if 0
	max_length = def->getAttrInt("max_length", -1);
	OmnString ss = def->getAttrStr("quote_escape_method");
	if (ss == "no_escape")
	{
		quote_escape_method = eNoEscape;
	}
	else if (ss == "escape_by_double")
	{
		quote_escape_method = eEscapeByDouble;
	}
	else if (ss == "escape_by_backslash")
	{
		quote_escape_method = eEscapeByBackslash;
	}
	else
	{
		AosLogError(rdata, true, AosErrmsgId::eInvalidEscapeMethod)
			<< AosFieldName::eValue1 << ss << enderr;
		return false;
	}
#endif

	return true;
}

