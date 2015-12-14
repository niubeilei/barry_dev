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
#include "semantics_c/rco.h"

#include <stdio.h>


aos_lock_t aos_rco_lock = PTHREAD_MUTEX_INITIALIZER;

int __aos_rco_hold(char * filename, 
				   const int lineno, 
				   void *holder, 
				   void *obj)
{
	// 
	// 'holder' holds the object 'obj'. We will keep track 
	// of this information. 
	//
	printf("Holder: %x ++: %x\n", 
			(unsigned int)holder, 
			(unsigned int)obj);
	return 0;
}

int chen() {return 0;}

int __aos_rco_put(char * filename, 
				   const int lineno, 
				   void *holder, 
				   void *obj)
{
	// 
	// 'holder' gives up the reference to 'obj'. We will 
	// remove the record. 
	//
	printf("Holder: %x --: %x\n", 
			(unsigned int)holder, 
			(unsigned int)obj);
	return 0;
}

