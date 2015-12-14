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
// 12/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/loc.h"

#include <string.h>


// 
// This function copies at most AOS_MAX_FILENAME_LEN-1
// If 'filename' is more than the allowed,
// it copies from the right location.
//
void aos_set_location(aos_location_t *loc, 
					  const char * const filename, 
					  const int lineno)
{
	int len = strlen(filename);
	int offset = 0;
	if (len >= AOS_MAX_FILENAME_LEN-1)
	{
		offset = len - AOS_MAX_FILENAME_LEN +1;
	}
	strcpy(loc->filename, filename+offset);
	loc->lineno = lineno;
}

