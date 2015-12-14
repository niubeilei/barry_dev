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
// 12/17/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semanobj_func_so_h
#define aos_semanobj_func_so_h

#include "semanobj/so_func.h"
#include "util_c/types.h"


struct aos_funcso;

extern int aos_funcso_func_to_call(struct aos_funcso *obj, 
								   const char * const filename, 
								   const int lineno, 
								   aos_time_t *start_time);
extern int aos_funcso_func_called(struct aos_funcso *obj, 
								   const char * const filename, 
								   const int lineno, 
								   const aos_time_t start_time);

extern int aos_funcso_func_entered(struct aos_funcso *obj, 
								   const char * const filename, 
								   const int lineno, 
								   aos_time_t *start_time);
extern int aos_funcso_func_exited(struct aos_funcso *obj, 
								   const char * const filename, 
								   const int lineno, 
								   const aos_time_t start_time, 
								   char * const exit_point);


extern int aos_funcso_stat(aos_funcso_t *obj, 
				   const char * const filename, 
				   const int lineno, 
				   const aos_time_t start_time);

extern int aos_funcso_log(aos_funcso_t *obj, 
				   const char * const filename, 
				   const int lineno, 
				   const aos_time_t start_time);

#endif // End of Include


