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
// 03/09/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_data_type_h
#define aos_util_data_type_h


typedef enum
{
	eAosDataType_Invalid,

	eAosDataType_Unknown,
	eAosDataType_NoDataType,
	eAosDataType_char,
	eAosDataType_int16,
	eAosDataType_int32,

	eAosDataType_int64,
	eAosDataType_u8,
	eAosDataType_u16,
	eAosDataType_u32,
	eAosDataType_u64,

	eAosDataType_string,
	eAosDataType_float,
	eAosDataType_double,
	eAosDataType_binary,
	eAosDataType_ip_addr, 
	eAosDataType_ptr, 

	eAosDataType_Buffer,

	eAosDataType_Last
} aos_data_type_e;

extern aos_data_type_e aos_data_type_2enum(const char * const str);
extern const char * aos_data_type_2str(const aos_data_type_e code);
extern int aos_data_type_is_int_type(const aos_data_type_e type);

static inline int aos_data_type_check(aos_data_type_e type)
{
	return (type > eAosDataType_Invalid && type < eAosDataType_Last)?1:0;
}

#endif

