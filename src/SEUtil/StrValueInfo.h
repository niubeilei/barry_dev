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
#ifndef Aos_SEUtil_StrValueInfo_h
#define Aos_SEUtil_StrValueInfo_h

#include "SEUtil/DataTooLongPolicy.h"
#include "SEUtil/DataTooShortPolicy.h"
#include "SEUtil/DataNullPolicy.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"


struct AosStrValueInfo
{
	enum
	{
		eNoEscape,
		eEscapeByDouble,
		eEscapeByBackslash
	};

	int							field_offset;
	int							field_data_len;
	int							field_len;
	AosDataTooLongPolicy::E		data_too_long_policy;
	AosDataTooShortPolicy::E	data_too_short_policy;
	AosDataNullPolicy::E		data_null_policy;
	char						front_padding;
	char						back_padding;
	char						null_padding;
	char						too_short_padding;
	char						too_long_padding;
	bool						set_field_null;
	bool						set_trailing_null;
	int							max_length;				// Chen Ding, 2014/12/04
	int							quote_escape_method;	// Chen Ding, 2014/12/04
	bool						notnull;

	AosStrValueInfo()
	:
	field_offset(-1),
	field_data_len(-1),
	field_len(-1),
	data_too_long_policy(AosDataTooLongPolicy::eTreatAsError),
	data_too_short_policy(AosDataTooShortPolicy::eTreatAsError),
	data_null_policy(AosDataNullPolicy::eTreatAsError),
	front_padding(0),
	back_padding(0),
	null_padding(0),
	too_short_padding(0),
	too_long_padding(0),
	set_field_null(true),
	set_trailing_null(false),
	max_length(-1),
	quote_escape_method(eNoEscape),
	notnull(false)
	{
	}

	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	void setPosition(const int offset, const int len)
	{
		field_offset = offset;
		field_data_len = len;
	}
};
#endif

