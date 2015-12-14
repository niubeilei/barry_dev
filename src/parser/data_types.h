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
// 01/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_parser_data_types_h
#define aos_parser_data_types_h

#define AOS_STRCMP_CASE_INSENSITIVE 0

typedef enum
{
	eAosLenType_Contents,
	eAosLenType_Fixed,
	eAosLenType_Cond
} aos_len_type_e;

typedef enum
{
	eAosPresType_Mandatory,
	eAosPresType_ForcedMandatory,
	eAosPresType_Optional,
	eAosPresType_Cond
} aos_presence_e;

typedef enum
{
	eAosDataType_int8,
	eAosDataType_int16,
	eAosDataType_int32,
	eAosDataType_int64,
	eAosDataType_uint8,
	eAosDataType_uint16,
	eAosDataType_uint32,
	eAosDataType_uint64,
	eAosDataType_string,
	eAosDataType_float,
	eAosDataType_double,
	eAosDataType_struct
} aos_data_type_e;

#endif

