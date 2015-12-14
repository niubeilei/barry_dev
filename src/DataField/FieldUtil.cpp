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
#include "DataField/FieldUtil.h"

#include "SEUtil/StrValueInfo.h"
#include "Rundata/Rundata.h"


bool 
AosFieldUtil::setFieldValue(
		char *record_data, 
		const int record_len, 
		char *value, 
		const int value_len, 
		const AosStrValueInfo &valinfo,
		AosRundata* rdata)
{
	// This function assumes the record 'record_data' has a field whose start
	// position is 'field_offset' and field length is 'field_len'. It
	// copies the value 'value' into the field. 
	// 1. If 'value' is shorter than 'field_len', it will either left align 
	//    (left_alignment == true) or right align (left_alignment == false) 
	//    the contents. 
	//    a. If it is left alignment, it sets the remaining bytes by 
	//       'back_padding'. 
	//    b. If it is right alignment, it sets the leading unused bytes 
	//       by 'front_padding'. 
	// 2. If 'value' is too long, the data is handled by 'data_too_long_policy'.
	//
	// Note that there are 'field_data_len' and 'field_len'. If no 'null' is 
	// appended at the end of a field, 'field_len' is always the same as
	// 'field_data_len'. Otherwise, 'field_len' is 'field_data_len' + 1.
	
	// bool set_field_null = (valinfo.field_len == valinfo.field_data_len + 1);
	
	bool rslt = true;
	/*
	if (!value || value_len <= 0)
	{
		memset(&record_data[valinfo.field_offset], valinfo.back_padding, valinfo.field_data_len);
		goto setTrailingNull;
	}
	*/

	if (value_len > valinfo.field_data_len)
	{
		// Data are too long. 
		switch (valinfo.data_too_long_policy)
		{
		case AosDataTooLongPolicy::eTreatAsError:
			 //AosSetErrorU(rdata, "data_too_long") << enderr;
			 memset(&record_data[valinfo.field_offset], valinfo.back_padding, valinfo.field_data_len);
			 rslt = false;
			 goto setTrailingNull;

		case AosDataTooLongPolicy::eResetRecord:
			 memset(&record_data[valinfo.field_offset], valinfo.too_long_padding, valinfo.field_data_len);
			 rslt = true;
			 goto setTrailingNull;

		case AosDataTooLongPolicy::eIgnoreSilently:
			 rslt = true;
			 goto setTrailingNull;
			 
		case AosDataTooLongPolicy::eTrimLeft:
			 memcpy(&record_data[valinfo.field_offset], &value[value_len - valinfo.field_data_len], 
					 valinfo.field_data_len);
			 rslt = true;
			 goto setTrailingNull;

		case AosDataTooLongPolicy::eTrimRight:
		default:
			 memcpy(&record_data[valinfo.field_offset], value, valinfo.field_data_len);
			 rslt = true;
			 goto setTrailingNull;
		}
	}
	else if (value_len < valinfo.field_data_len)
	{
		int delta = valinfo.field_data_len - value_len;
		switch (valinfo.data_too_short_policy)
		{
		case AosDataTooShortPolicy::eTreatAsError:
			 //AosSetErrorU(rdata, "data_too_short") << enderr;
			 memset(&record_data[valinfo.field_offset], valinfo.back_padding, valinfo.field_data_len);
			 rslt = false;
			 goto setTrailingNull;

		case AosDataTooShortPolicy::eIgnoreSilently:
			 rslt = true;
			 goto setTrailingNull;
			 
		case AosDataTooShortPolicy::eCstr:
			 // Ketty 2014/01/11
			 rslt = true;
			 memcpy(&record_data[valinfo.field_offset], value, value_len);
			 record_data[valinfo.field_offset + value_len] = 0;
			 return rslt;

		case AosDataTooShortPolicy::eAlignRight:
			 memset(&record_data[valinfo.field_offset], valinfo.front_padding, delta);
			 memcpy(&record_data[valinfo.field_offset+delta], value, value_len);
			 rslt = true;
			 goto setTrailingNull;

		case AosDataTooShortPolicy::eAlignLeft:
		default:
			 memcpy(&record_data[valinfo.field_offset], value, value_len);
			 memset(&record_data[valinfo.field_offset+value_len], valinfo.back_padding, delta);
			 if (valinfo.set_field_null) record_data[valinfo.field_offset + value_len] = 0;
			 rslt = true;
			 goto setTrailingNull;
		}
	}
	else
	{
		// The same length as 'field_data_le'. 
		memcpy(&record_data[valinfo.field_offset], value, value_len);
		rslt = true;
		goto setTrailingNull;
	}

setTrailingNull:
	if (valinfo.set_trailing_null) record_data[valinfo.field_offset + valinfo.field_len] = 0;
	return rslt;
}

