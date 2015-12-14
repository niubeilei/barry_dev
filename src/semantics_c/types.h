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
// Object Dictionary keeps track of objects. Objects are identified
// by their starting addresses. 
//   
//
// Modification History:
// 01/18/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_types_h
#define aos_semantics_types_h

typedef enum
{
	eAosDataType_Unknown,

	eAosDataType_CHAR,
	eAosDataType_SCHAR,
	eAosDataType_UCHAR,
	eAosDataType_SHORT,
	eAosDataType_USHORT,
	eAosDataType_INT,
	eAosDataType_UINT,
	eAosDataType_INT64,
	eAosDataType_UINT64,
	eAosDataType_STRING,
	eAosDataType_BOOL,
	eAosDataType_FLOAT,
	eAosDataType_DOUBLE,
	eAosDataType_ARRAY,
	eAosDataType_USERDEFINED
} aos_data_type_e;

typedef enum
{
	eAosEntryType_Unknown,

	eAosEntryType_Array
} aos_entry_type_e;


#endif // End of Include

