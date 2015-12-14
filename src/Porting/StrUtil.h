////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrUtil.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_StrUtil_h
#define Omn_Porting_StrUtil_h

#ifdef OMN_PLATFORM_MICROSOFT

#include <stdlib.h>
#include "Util/String.h"

class OmnStrUtil 
{
public:
	static OmnString	itoa(const int v)
	{
		char buf[10];
		_itoa(v, buf, 10);
		return OmnString(buf);
	}

	static void 	replaceSubstr(char *target,
					   			  const int targetLength, 
								  const int targetMaxLength,
								  const int replacedPos,
								  const int replacedLength,
								  const char *replacingStr,
								  const int replacingLength);
};

#elif OMN_PLATFORM_UNIX

#include "Util/String.h"
#include <stdlib.h>
#include <stdio.h>

class OmnStrUtil
{
public:
	static OmnString itoa(const int v)
	{
    	char buf[10];
    	sprintf(buf, "%d", v);
    	return OmnString(buf);
	}

	static OmnString ltoa(const long v)
	{
    	char buf[20];
    	sprintf(buf, "%d", (int)v);
    	return OmnString(buf);
	}

	static OmnString lltoa(const long long v)
	{
    	char buf[20];
    	sprintf(buf, "%lld", v);
    	return OmnString(buf);
	}

	static OmnString ulltoa(const unsigned long long v)
	{
    	char buf[20];
    	sprintf(buf, "%llu", v);
    	return OmnString(buf);
	}

	static OmnString dtoa(const double v)
	{
    	char buf[20];
    	sprintf(buf, "%.2f", v);
    	return OmnString(buf);
	}

	static int itoa(char *data, const int v)
	{
		//
		// It converts the value 'v' into a string, stored in 'data'.
		// It returns the length of the converted string.
		//
		if (v < 1000)
		{
			sprintf(data, "%4d", v);
			return 4;
		}

		if (v < 100000)
		{
			sprintf(data, "%6d", v);
			return 6;
		}

		sprintf(data, "%10d", v);
		return 10;
	}	


	static int ltoa(char *data, const long v)
	{
		//
        // It converts the value 'v' into a string, stored in 'data'.
        // It returns the length of the converted string.
        //
        if (v < 1000)
        {
            sprintf(data, "%4ld", v);
            return 4;
        }

        if (v < 100000)
        {
            sprintf(data, "%6ld", v);
            return 6;
        }

		if (v < 1000000000)
		{
        	sprintf(data, "%10ld", v);
        	return 10;
		}

		sprintf(data, "%20ld", v);
		return 20;
    }

	static void 	replaceSubstr(char *target,
					   			  const int targetLength, 
								  const int targetMaxLength,
								  const int replacedPos,
								  const int replacedLength,
								  const char *replacingStr,
								  const int replacingLength);

	static OmnString assemble(const uint &high, const ulong &low)
	{
		//
		// This function assembles a string by converting 'high'
		// to the first four bytes, and 'low' to the lower 8 bytes.
		//
		char buf[20];
		sprintf(buf, "%d", high);
		sprintf(&buf[4], "%ld", low);
		buf[12] = 0;
		return OmnString(buf);
	}

	// 
	// Chen Ding, 05/07/2003
	//
	static bool getDomainName(const OmnString &mgcpEid,
						  	  OmnString &domainName,
						  	  OmnString &err);
};

#endif

#endif
