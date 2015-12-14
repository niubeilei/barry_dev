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
// 05/20/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_Charset_h
#define Omn_Util_Charset_h

#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include <vector>


class AosCharset
{
public:
	enum Flag
	{
		eInvalidFlag = 0,

		eAllowFlag, 
		eFinishFlag,
		eNotAllowedFlag,

		eMaxFlag
	};

	struct Entry
	{
		int 	start;
		int		end;
		Flag 	flag;
	};

	enum ErrorCode
	{
		eNoError,
	
		eTooShort,
		eInvalidChar,
		eInternalError,
		eTooLong,
		eReachEOF
	};

private:
	enum
	{
		eCharsetSize = 256
	};

	bool	mAllowEOF;
	int		mMinLen;
	int		mMaxLen;
	bool	mCharset[eCharsetSize];

public:

	AosCharset();
	AosCharset(const std::vector<Entry> &def);

	Flag getStrToEnum(const OmnString flag);

	bool config(AosRundata *rdata, const AosXmlTagPtr &def);

	bool nextWord(
				const char *data, 
				const int len, 
				int &idx, 
				int &start, 
				int &word_len,
				char &invalid_char,
				ErrorCode &errcode) const;

	int parseWords(const char *data, 
				const int len, 
				std::vector<OmnString> &words, 
				ErrorCode &errcode);

private:
	bool init(const vector<Entry> &def);
};

#endif
