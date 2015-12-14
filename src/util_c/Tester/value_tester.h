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
// 04/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_tester_value_tester_h
#define aos_util_tester_value_tester_h

#include "util_c/types.h"
#include "util_c/data_type.h"

struct aos_value;

#define AOS_VALUE_ICO_MAX_STR_LEN 1000000

typedef struct aos_value_ico
{
	aos_data_type_e	type;
	int64_t			int_value;
	u64				u64_value;
	char 			str_value[AOS_VALUE_ICO_MAX_STR_LEN+1];
	int				data_len;
	double			double_value;
	void *			ptr;
} aos_value_ico_t;

extern int aos_value_tester();
extern int aos_value_ico_set_value(aos_value_ico_t *obj, aos_data_type_e type, void *value, int len);
extern int aos_value_ico_comp(aos_value_ico_t *obj, struct aos_value *value);
extern int aos_value_ico_init(aos_value_ico_t *obj);
#endif

