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
// RandomSelStr randomly selects an integer from a list based on a
// random selection model. As an example, below is a random distribution
// model:
//     Value      Weight
//     string1    10%
//     string2    5%
//     string3    15%
//     string4    20%
//     string5    50%
//
// Given this random selection model, this class will randomly select 
// strings  based on the weights. 
//
// Modification History:
// 11/26/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomSelStr_h
#define Omn_Random_RandomSelStr_h

#include "aosUtil/Types.h"
#include "Random/RandomSelector.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"
#include "Util/StrType.h"
#include "XmlParser/Ptrs.h"


class AosRandomSelStr : public AosRandomSelector
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultCorrectSepPct = 80,
		eDefaultWithTrailingSepPct = 50,
		eDefaultMinValues = 1,
		eDefaultMaxValues = 5,
		eDefaultIncorrectStrMin = 1, 
		eDefaultIncorrectStrMax = 20, 
		eMaxWeight = 1000, 

		eViolateUnique = 0,   
		eViolateOrdered = 1, 
		eViolateMinValues = 2,
		eViolateMaxValues = 3,
		eIncorrect = 4,
		eNullStr = 5,
		eNotInList = 6,

		eDefaultViolateUniqueWeight = 50,
		eDefaultViolateOrderedWeight = 50, 
		eDefaultViolateMinValuesWeight = 50,
		eDefaultViolateMaxValuesWeight = 50,
		eDefaultIncorrectWeight = 20,
		eDefaultNullStrWeight = 10, 
		eDefaultNotInListWeight = 5
	};

	u32		mViolateUniqueWeight;
	u32		mViolateOrderedWeight;
	u32		mViolateMinValuesWeight;
	u32		mViolateMaxValuesWeight;
	u32		mIncorrectWeight;
	u32		mNullStrWeight;
	u32		mNotInListWeight;


    OmnDynArray<OmnString, 100, 100, 10000>	mStrs;
	OmnDynArray<u16, 100, 100, 10000>		mWeights;
	OmnDynArray<bool, 100, 100, 10000>		mFlags;
	OmnDynArray<u32, 100, 100, 10000>		mTags;

	u32								mTotalWeight;
	char							mSeparator;
	u32								mMin;
	u32								mMax;
	bool							mUnique;
	bool							mOrdered;
	u32								mIncorrectStrMin;
	u32								mIncorrectStrMax;

	u32								mCorrectSepPct;
	u32								mWithTrailingSepPct;

	AosStrType						mInvalidStrType;
	OmnString						mBadValue;
    AosRandomIntegerPtr				mBadValueSelector;

public:
	AosRandomSelStr();
	AosRandomSelStr(const OmnString &values);
	AosRandomSelStr(const OmnString &name, const OmnXmlItemPtr &conf);
	~AosRandomSelStr();

	bool				config(const OmnXmlItemPtr &conf);
	virtual int 		nextInt();	
	virtual int 		nextInt(AosGenTable &data);	
	virtual int 		nextInt(const OmnString &name);	
	virtual int 		nextInt(const OmnString &name, AosGenTable &data);	
	virtual OmnString	nextStr();
	virtual OmnString	nextStr(const OmnString &name);
	virtual OmnString	nextStr(AosGenTable &data);
	virtual OmnString	nextStr(const OmnString &name, AosGenTable &data);
	int					getNumElems() const {return mWeights.entries();}

	bool				setSeparator(const char sep, const u32 min, const u32 max);
	OmnString 			nextStrWithSeparator();
	OmnString 			nextIncorrectWithSeparator(
							AosRVGReturnCode &rcode, 
							OmnString &errmsg);
	OmnString 			nextIncorrectStr(AosRVGReturnCode &rcode, 
							OmnString &errmsg);
	OmnString 			nextIncorrectStrPriv(
							AosRVGReturnCode &rcode, 
							OmnString &errmsg);
	OmnString 			nextIncorrectSep(
							AosRVGReturnCode &rcode, 
							OmnString &errmsg);
	bool 				parseSeparator(const OmnXmlItemPtr &sepDef);
	bool 				parseIncorrectGen(const OmnXmlItemPtr &def);

private:
	bool		set(OmnString *strs,  u16 *weights, const u32 numElems);
	OmnString	nextStrPriv();
	bool		addValue(const OmnString &value, u32 weight);
	bool 		createSelector();
	bool 		makeStrHaveEnoughSegs(OmnString &str, const u32 min);
	OmnString	notInList(
					AosRVGReturnCode &rcode,
					OmnString &errmsg);
	bool		makeNotOrdered(
					OmnString &str, 
					AosRVGReturnCode &rcode,
					OmnString &errmsg);
	bool 		makeNotUnique(OmnString &str, 
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	bool 		makeSureValueIsInvalid(
					OmnString &str, 
					AosRVGReturnCode &rcode, 
					OmnString &errmsg);
	bool 		parseValues(const OmnString &values);
};

#endif

