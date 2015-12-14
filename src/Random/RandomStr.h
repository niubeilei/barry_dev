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
// RandomStr randomly generates strings of various types. The attributes
// for a random string include:
// 1. Minimum Length
// 2. Maximum Length
// 3. Types of Strings
//
// Modification History:
// 03/09/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomStr_h
#define Omn_Random_RandomStr_h

#include "aosUtil/Types.h"
#include "Random/RandomUtil.h"
#include "Random/RandomObj.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosRandomStr : public AosRandomObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eCharsetMaxSize = 256,
		eDefaultMaxLen = 10000
	};

private:
	OmnString	mName;
	u32			mMin;
	u32			mMax;

	u32 		mLeadMin;
	u32			mLeadMax;
	u32			mLeadCharsetSize;
	char		mLeadCharset[eCharsetMaxSize];

	u32			mBodyCharsetSize;
	char		mBodyCharset[eCharsetMaxSize];

	u32 		mTailMin;
	u32			mTailMax;
	u32			mTailCharsetSize;
	char		mTailCharset[eCharsetMaxSize];

	AosStrType	mStrType;

public:
	AosRandomStr();
	AosRandomStr(const OmnString &name, 
				 const u32 min, 
				 const u32 max, 
				 const AosStrType strtype); 
	AosRandomStr(const OmnXmlItemPtr &conf);
	~AosRandomStr();

	virtual int 		nextInt();	
	virtual int 		nextInt(AosGenTable &data);	
	virtual int 		nextInt(const OmnString &name);	
	virtual int 		nextInt(const OmnString &name, AosGenTable &data);	
	virtual OmnString	nextStr();
	virtual OmnString	nextStr(const u32 min, const u32 max);
	virtual OmnString	nextStr(const OmnString &name);
	virtual OmnString	nextStr(AosGenTable &data);
	virtual OmnString	nextStr(const OmnString &name, AosGenTable &data);

	bool	checkStr(OmnString &str);
	bool	initCharset();
	bool 	generateStr(OmnString &str, const u32 min, const u32 max, 
					   const char *cs, const u32 charsetSize);
	u32 	setCharset(char *charset, 
					   const u32 startIndex, 
					   const char startChar, 
					   const char endChar);
	bool	setCharset(const u32 leadmin, const u32 leadmax, 
					   const char *leadcs, const u32 leadCharsetSize, 
					   const char *bodycs, const u32 bodyCharsetSize, 
					   const u32 tailmin, const u32 tailmax, 
					   const char *tailcs, const u32 tailCharsetSize);

private:
};

#endif

