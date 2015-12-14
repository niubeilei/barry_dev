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
#ifndef Aos_semanopr_ptrtype_h
#define Aos_semanopr_ptrtype_h

#include "semanopr/ptrtype.h"


typedef enum 
{
	eAosPtrType_Null, 
	eAosPtrType_Unknown, 
	eAosPtrType_Valid, 
	eAosPtrType_Invalid
} aos_ptrtype_e;

extern char * aos_ptrtype_to_str(aos_ptrtype_e code);
extern aos_ptrtype_e aos_ptrtype_to_enum(const char * const name);


#endif // End of Include

