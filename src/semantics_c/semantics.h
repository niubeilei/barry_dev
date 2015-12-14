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
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_semantics_h
#define aos_semantics_semantics_h

#include "semanopr/memopr.h"
#include "semantics_c/memory.h"

#define AOS_MAX_DATA_TYPE_LEN	20

extern int aos_semantics_init();
extern char * aos_semantics_get_logdir();
extern void aos_semantics_set_logdir(const char * const logdir);
extern void aos_semantics_proc_cmdline(int *argc, char **argv);

#endif // End of Include

