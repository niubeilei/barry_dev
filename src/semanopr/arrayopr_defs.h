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
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_semanopr_arrayopr_defs_h
#define Aos_semanopr_arrayopr_defs_h

#include "semantics_c/semantics.h"


extern int __aos_array_decl(const char * const filename, 
							int lineno, 
							void *array, 
							const char * const data_type, 
							const aos_mat_t alloc_type,
							const int size);

extern int __aos_array_delete(char *filename, 
							int lineno, 
							char *array);

extern int __aos_array_modify(char *filename, 
							int lineno, 
							void *array, 
							int index); 

extern int __aos_array_read(char *filename, 
							int lineno, 
							void *array, 
							int index);

extern int __aos_array_stop(char *filename, 
							int lineno, 
							char *array);

extern int __aos_array_resume(char *filename, 
							int lineno, 
							char *array);

extern int __aos_array_read_only(char *filename, 
							int lineno, 
							char *array);

extern int __aos_array_modify_only(char *filename, 
							int lineno, 
							char *array);

extern int __aos_array_read_modify(char *filename, 
							int lineno, 
							char *array);

#endif // End of Include

