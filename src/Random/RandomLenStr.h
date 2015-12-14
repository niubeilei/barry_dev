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
// RandomInteger randomly selects an integer from a list based on a
// random selection model. As an example, below is a random distribution
// model:
//     StringType
//     Value      Weight
//     1          10%
//     [2, 9],    15%
//     10,        20%
//     [11, 50],  15%
//     [51, 10000], 25%
//     [10001, -], -%
//
// Given this random selection model, this class will randomly select 
// [1, 100] based on the weights. 
//
// Modification History:
// 12/04/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomLenStr_h
#define Omn_Random_RandomLenStr_h

#include "aosUtil/Types.h"
#include "Random/RandomSelectorLenStr.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosRandomLenStr : public AosRandomSelectorLenStr
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxSelectors = 100
	};

	enum
	{
		eStr = 0,
		eAsciiStr,
		eBinStr,
		eLowcaseStr,
		eCapitalStr,
		eDigitStr,
		eLetterStr,
		eLetterDigitStr,
		eVarNameStr
	};

private:
	int         		mStrType;
	AosRandomIntegerPtr	mLenSelector;

public:
	AosRandomLenStr();
	AosRandomLenStr(const OmnString &name, 
					const int strTypeEnum, 
					const OmnXmlItemPtr &conf, 
					const u32 base);
	~AosRandomLenStr();

	virtual int 		nextInt();	
	virtual int 		nextInt(AosGenTable &data);	
	virtual int 		nextInt(const OmnString &name);	
	virtual int 		nextInt(const OmnString &name, AosGenTable &data);	
	virtual OmnString	nextStr();
	virtual OmnString	nextStr(const OmnString &name);
	virtual OmnString	nextStr(AosGenTable &data);
	virtual OmnString	nextStr(const OmnString &name, AosGenTable &data);

	int         getNumElems() const;

private:

};

#endif

