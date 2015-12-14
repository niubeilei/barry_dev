////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Atol.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Atol_h
#define Omn_Porting_Atol_h

#include <stdlib.h>

#ifdef OMN_PLATFORM_UNIX
inline long long OmnAtoll(const char *buf)
{
	return atoll(buf);
}

inline unsigned long OmnAtoul(const char *buf)
{
	char *tmp;
	return strtoul(buf, &tmp, 10);
}

inline unsigned long long OmnAtoull(const char *buf)
{
	char *tmp;
	return strtoull(buf, &tmp, 10);
}

#elif OMN_PLATFORM_MICROSOFT

inline __int64 OmnAtoll(const char *buff)
{
	return _atoi64(buff);
}

#endif

#endif

