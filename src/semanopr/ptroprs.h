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
#ifndef Aos_semanopr_ptropr_h
#define Aos_semanopr_ptropr_h

#ifndef AOS_SEMANTICS

#define aos_pointer_decl(pointer, pointer_type, data_type, value)

#else
#include "semantic_operators/array_operators_def.h"

#define aos_pointer_decl(pointer, pointer_type, data_type, value) 	\
			__aos_pointer_decl(__FILE__, __LINE__, 					\
					pointer, pointer_type, data_type);

#endif // AOS_SEMANTICS

#endif // End of Include

