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
// 03/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_conditions_types_h
#define aos_conditions_types_h

//////////////// aos_cond_type_e ////////////////////////
typedef enum
{
	eAosCondType_Invalid = 0,

	eAosCondType_Const,

	eAosCondType_Last
} aos_cond_type_e;

static inline int aos_cond_type_check(aos_cond_type_e type)
{
	return (type > eAosCondType_Invalid && type < eAosCondType_Last);
}

extern const char *aos_cond_type_2str(const aos_cond_type_e type);
extern aos_cond_type_e aos_cond_type_2enum(const char * const type);


#endif

