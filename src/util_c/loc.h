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
#ifndef aos_util_loc_h
#define aos_util_loc_h

#define AOS_MAX_FILENAME_LEN	50

typedef struct
{
	char 		filename[AOS_MAX_FILENAME_LEN];
	int			lineno;
} aos_location_t;

extern void aos_set_location(aos_location_t *loc, 
					  const char * const filename, 
					  const int lineno);

#endif // End of Include

