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
//
// Modification History:
// 02/22/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/strutil1.h"

#include "util2/global_data.h"
#include <stdio.h>


char * aos_str_int_to_str(const int64_t value)
{
	int len;
    char *str = aos_get_global_str1(&len);
    sprintf(str, "%lld", (long long)value);
    return str;
}

