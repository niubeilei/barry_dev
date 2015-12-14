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
#ifndef Aos_semanopr_ptropr_defs_h
#define Aos_semanopr_ptropr_defs_h

#include "semanopr/ptrtype.h"


extern int __aos_pointer_decl(const char * const filename, 
					const int lineno, 
					void *pointer, 
					aos_ptrtype_e pointer_type, 
					char * data_type, 
					void *value);


#endif // End of Include

