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
// 2014/09/16, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_Charset_h
#define Aos_SEUtil_Charset_h

#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

class AosCharset
{
private:
	enum
	{
		eCharsetSize = 256
	};

	int			mMinLen;
	int			mMaxLen;
	int			mPrefixMinLen;
	int			mPostfixMinLen;
	bool		mPrefixCharset[eCharsetSize];
	bool		mBodyCharset[eCharsetSize];
	bool		mPostfixCharset[eCharsetSize];

public:
	AosCharset();
	AosCharset(const AosXmlTagPtr &conf);

	bool isValid(AosRundata *rdata, const OmnString &str) const;
};

#endif
