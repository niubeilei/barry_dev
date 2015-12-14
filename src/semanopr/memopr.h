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
#ifndef Aos_semanopr_memopr_h
#define Aos_semanopr_memopr_h

#include "util_c/errmgr.h"
#include "semanopr/memopr_defs.h"

#ifndef AOS_SEMANTICS

#define aos_verify_cast(entry, type)

#else

#define aos_verify_cast(entry, type) 						\
 	if (entry) __aos_verify_cast(__FILE__, __LINE__, 		\
 					entry->__aos_semantic_iden, type);		\
 	else aos_errmgr_nullptr(__FILE__, __LINE__)

#endif	// end of AOS_SEMANTICS

#endif // End of Include

