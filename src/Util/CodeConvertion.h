////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// Description:
////
//// Modification History:
////  07/08/2011  Created by Tracy Huang 
//////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_CodeConvertion_h
#define Omn_Util_CodeConvertion_h

#include <iconv.h>

#include "Util/String.h"


class CodeConvertion
{
public:
	const static OmnString mDefaultType;

	static bool isValid(const OmnString &type)
	{
		iconv_t v = iconv_open(mDefaultType.data(), type.data());
		iconv_close(v);
		if (v == (iconv_t)-1) return false;
		return true;
	}

public:
	CodeConvertion(){}
	CodeConvertion(const char *from_charset, const char *to_charset)
	{
		mI_cd = iconv_open(to_charset, from_charset);
		//	if((iconv_t)-1 == i_cd)  printf("iconv open error!\n");
	}
	~CodeConvertion()
	{
		if (mI_cd)
			iconv_close(mI_cd);
	}
public:
	size_t translate(char *src, size_t srcLen, char *desc, size_t descLen);
	static size_t convert(const char *from_charset, const char *to_charset,
			char *src, size_t srcLen, char *desc, size_t descLen);
private:
	iconv_t mI_cd;
};

#endif
