////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 02/02/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/data_type.h"

#include "alarm_c/alarm.h"

#include <string.h>



aos_data_type_e aos_data_type_2enum(const char * const str)
{
	aos_assert_r(str, eAosDataType_Invalid);

	if (strcmp(str, "unknown") == 0) return eAosDataType_Unknown;
	if (strcmp(str, "no_data_type") == 0) return eAosDataType_NoDataType;
	if (strcmp(str, "char") == 0) return eAosDataType_char;
	if (strcmp(str, "int16") == 0) return eAosDataType_int16;
	if (strcmp(str, "int32") == 0) return eAosDataType_int32;
	if (strcmp(str, "int64") == 0) return eAosDataType_int64;
	if (strcmp(str, "u8") == 0) return eAosDataType_u8;
	if (strcmp(str, "u16") == 0) return eAosDataType_u16;
	if (strcmp(str, "u32") == 0) return eAosDataType_u32;
	if (strcmp(str, "u64") == 0) return eAosDataType_u64;
	if (strcmp(str, "string") == 0) return eAosDataType_string;
	if (strcmp(str, "float") == 0) return eAosDataType_float;
	if (strcmp(str, "double") == 0) return eAosDataType_double;
	if (strcmp(str, "binary") == 0) return eAosDataType_binary;
	if (strcmp(str, "ip_addr") == 0) return eAosDataType_ip_addr;
	if (strcmp(str, "ptr") == 0) return eAosDataType_ptr;

	return eAosDataType_Invalid;
}


const char * aos_data_type_2str(const aos_data_type_e type)
{
	switch (type)
	{
	case eAosDataType_Unknown:
		 return "unknown";

	case eAosDataType_NoDataType:
		 return "no_data_type";

	case eAosDataType_char:
		 return "char";

	case eAosDataType_int16:
		 return "int16";

	case eAosDataType_int32:
		 return "int32";

	case eAosDataType_int64:
		 return "int64";

	case eAosDataType_u8:
		 return "u8";

	case eAosDataType_u16:
		 return "u16";

	case eAosDataType_u32:
		 return "u32";

	case eAosDataType_u64:
		 return "u64";

	case eAosDataType_string:
		 return "string";

	case eAosDataType_float:
		 return "float";

	case eAosDataType_double:
		 return "double";

	case eAosDataType_binary: 
		 return "binary";

	case eAosDataType_ip_addr:
		 return "ip_addr";

	case eAosDataType_ptr:
		 return "ptr";

	default: 
		 aos_alarm("Unrecognized type: %d", type);
		 return "Incorrect";
	}

	aos_should_never_come_here;
	return "incorrect";
}


int aos_data_type_is_int_type(const aos_data_type_e type)
{
	if (type == eAosDataType_char ||
		type == eAosDataType_int16 ||
		type == eAosDataType_int32 ||
		type == eAosDataType_int64 ||
		type == eAosDataType_u8 ||
		type == eAosDataType_u16 ||
		type == eAosDataType_u32 ||
		type == eAosDataType_u64) return 1;
	return 0;
}


