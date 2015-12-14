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
// 12/12/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_aos_util_errmgr_priv_h
#define Omn_aos_util_errmgr_priv_h

#include "aosUtil/Types.h"

typedef struct
{
	char *  filename;
	int     lineno;
	u32     __aos_semantic_iden;
	char *  errmsg;
	u32		count;
} aos_errmgr_entry_t;

extern aos_errmgr_entry_t *
aos_errmgr_create_errentry(const char * const filename, 
						   const int lineno, 
						   const char * const errmsg);

#endif

