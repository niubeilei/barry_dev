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
//     Value      Weight
//     [1, 4]     10%
//     [5, 20]    5%
//     [21, 50]   15%
//     [51, 88]   20%
//     [89, 100]  50%
//
// Given this random selection model, this class will randomly select 
// [1, 100] based on the weights. 
//
// Modification History:
// 11/26/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomInteger_h
#define Omn_Random_RandomInteger_h

#include "aosUtil/Types.h"
#include "Random/RandomSelector.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosRandomInteger : public AosRandomSelector
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxSelectors = 10000,
		eWeightBase = 100
	};

private:
    int64_t	  * mLows;
    int64_t	  *	mHighs;
	u16       * mWeights;
	u32 		mNumElems;

public:
	AosRandomInteger(const u32 base);
	AosRandomInteger(const OmnString &name, const OmnString &conf, const u32 base);
	AosRandomInteger(const OmnString &name, const OmnXmlItemPtr &conf, const u32 base);
	AosRandomInteger(
			int64_t * low,
			int64_t * high,
			u16 * weights,
			const u32 numElems,
			const u32 base);

	~AosRandomInteger();

	bool				config(const OmnXmlItemPtr &conf);
	virtual int 		nextInt();	
	virtual int 		nextInt(AosGenTable &data);	
	virtual int 		nextInt(const OmnString &name);	
	virtual int 		nextInt(const OmnString &name, AosGenTable &data);	
	virtual OmnString	nextStr();
	virtual OmnString	nextStr(const OmnString &name);
	virtual OmnString	nextStr(AosGenTable &data);
	virtual OmnString	nextStr(const OmnString &name, AosGenTable &data);

	int			getNumElems() const {return mNumElems;}
	bool		set(int64_t *low, int64_t *high, u16 *weights, const u32 numElems);

private:

	bool checkLowHigh(int low, int high, const OmnXmlItemPtr &conf);
	bool checkWeight(int weight, const OmnXmlItemPtr &conf);
	bool createArrays();
};

#endif

