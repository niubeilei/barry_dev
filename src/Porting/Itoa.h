////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Itoa.h
// Description:
//	It defines platform dependent functions:
//		OmnItoa(int);   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Itoa_h
#define Omn_Porting_Itoa_h

#include <stdio.h>
#include "Util/String.h"


#ifdef OMN_PLATFORM_UNIX
//
// It is Unix platform
//
#elif OMN_PLATFORM_MICROSOFT
//
// It is Microsoft platform
//
inline OmnString OmnItoa(const int value)
{
	char buf[10];
	sprintf(buf, "%d", value);
	return OmnString(buf);
}
#endif
#endif




