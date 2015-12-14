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
// 02/29/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_parser_attr_h
#define aos_omni_parser_attr_h

#define AOS_ATTR_NAME_MAX_LEN 32
#define AOS_ATTR_VALUE_MAX_LEN 64

typedef struct aos_attr1
{
	char		name[AOS_ATTR_NAME_MAX_LEN];
	char		*value;
} aos_attr1_t;

extern int aos_attr_init(aos_attr1_t *attr);
extern int aos_attr_free_all(aos_attr1_t **attr, const int num_attr);
extern int aos_attr_copy(aos_attr1_t *to, aos_attr1_t *from);
extern int aos_attr_set(
		aos_attr1_t *attr, 
		const char * const name, 
		const char * const value);

#endif

