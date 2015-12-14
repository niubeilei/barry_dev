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
// 11/24/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_Asserts_h
#define Aos_Semantics_Asserts_h

#ifndef AOS_SEMANTICS

#define aos_semantic_assert(cond)

#else
#include "Debug/ErrorMgr.h"

#define aos_semantic_assert(cond) 										\
	if (!cond) AosErrorMgrSelf->addError(__FILE__, __LINE__,  			\
			"Semantic Asserts failed", "", OmnGetCurrentThreadId());

#endif

#endif
