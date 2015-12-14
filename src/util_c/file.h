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
// 03/06/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_file_h
#define aos_util_file_h

#ifdef __cplusplus
extern "C" {
#endif

extern int aos_file_read_file(
		const char * const fn, 
		char **contents, 
		int *len);

extern int aos_file_write_to_file(
		const char * const fn, 
		const char * const contents, 
		const int len);

extern int aos_file_append_to_file(
		const char * const fn, 
		const char * const contents, 
		const int len);

#ifdef __cplusplus
}
#endif

#endif // End of Include

