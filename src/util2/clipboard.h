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
// 03/09/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util2_clipboard_h
#define aos_util2_clipboard_h

#include "util_c/data_type.h"


extern int aos_clipboard_init();

extern int aos_clipboard_set_str(
		const aos_data_type_e type, 
		const char * const name, 
		char *ptr, 
		const int override_flag);

extern int aos_clipboard_set_ptr(
		const aos_data_type_e type, 
		const char * const name, 
		void *ptr, 
		const int override_flag);

extern int aos_clipboard_set_int(
		const aos_data_type_e type, 
		const char * const name, 
		const int value, 
		const int override_flag);

extern int aos_clipboard_get_str(
		const aos_data_type_e type, 
		const char * const name, 
		char **ptr, 
		int *len);

extern int aos_clipboard_get_str_b(
		const aos_data_type_e type, 
		const char * const name, 
		char *str, 
		int *len);

extern int aos_clipboard_get_ptr(
		const aos_data_type_e type, 
		const char * const name, 
		void **ptr); 

extern int aos_clipboard_get_int(
		const aos_data_type_e type, 
		const char * const name, 
		int *value); 

#endif // End of Include

