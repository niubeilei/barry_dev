//////////////////////////////////////////////////////////////////////////
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
// Modification History:
//  07/08/2011  Created by Tracy Huang 
//////////////////////////////////////////////////////////////////////////////

#include "Util/CodeConvertion.h"

#include "Debug/Debug.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


const OmnString CodeConvertion::mDefaultType = "UTF8";

static int sgTotalAlarms = 1000;

size_t CodeConvertion::translate(char *src, size_t srcLen, char *desc, size_t descLen)
{
	char **inbuf = &src;
	char **outbuf = &desc;
	memset(desc, 0, descLen);
	return iconv(mI_cd, inbuf, &srcLen, outbuf, &descLen);
}

size_t CodeConvertion::convert(const char *from_charset, const char *to_charset,
		char *src, size_t srcLen, char *desc, size_t descLen)
{
	char **inbuf = &src;
	char **outbuf = &desc;
	iconv_t cd = iconv_open(to_charset, from_charset);
	if ((iconv_t)-1 == cd) return (size_t)-1;
	memset(desc, 0, descLen);
	size_t slen = srcLen;
	size_t dlen = descLen;
	while(slen > 0)
	{
		size_t n = iconv(cd, inbuf, &slen, outbuf, &dlen);
		if (n == (size_t)-1)
		{
			if (errno == EILSEQ || errno == EINVAL)
			{
				(*inbuf)++;
				slen--;
				*(*outbuf) = '?';
				++(*outbuf);
				dlen--;	
				if (sgTotalAlarms > 0)
				{
					sgTotalAlarms--;
					OmnScreen << "Coding convertion faild: " << strerror(errno) << endl;
				}
			}
			else if (errno == E2BIG)
			{
				if (sgTotalAlarms > 0)
				{
					OmnScreen << "Coding convertion faild: " << strerror(errno) << endl;
					sgTotalAlarms--;
				}

				iconv_close(cd);
				return -1;
			}
		}
	}
	iconv_close(cd);
	return descLen - dlen;
}

