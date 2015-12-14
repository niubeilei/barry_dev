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
// This class makes a string frome a charset:
//   [(char1, weight1), (char2, weight2), ..., (charn, weightn)]
// It randomly generates string that consist of the chars
// selected from the charset. selected Chars are concatenated together. 
// The charset may have some constraints:
// 1. Number of Chars: [min, max]
//    The generated string shall contain at least 'min' number of values and
//    at most 'max' number of values. 
// 2. Unique: true|false
//    Chars in a generated string may or may not be repeated.
// 3. Order: true|false
//    Chars may be ordered or not ordered.
//   
//
// Modification History:
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef  Aos_RVG_CharsetRSG_h
#define  Aos_RVG_CharsetRSG_h

#include "RVG/RSG.h"
#include "RVG/RIG.h"
#include <functional>

// 
//        Class:  AosCharsetRSG
//  Description:  charset based Random String Generator 
// 
class AosCharsetRSG : public AosRSG
{
	OmnDefineRCObject;
public:
	enum
	{
		eCharsetRSGDefaultWeight = 100
	};
	typedef struct CharWeight
	{
		char mChar;
		u32 mWeight;
		u32 mCreateTimes;
	} CharWeight;

	//
	// a functor to find char, used by find/remove algorithm
	//
	typedef struct FunctorFindChar: public std::binary_function<CharWeight, char, bool>
	{
		bool operator()(const CharWeight& charWeight, const char charactor) const
		{
			return (charWeight.mChar == charactor);
		}
		
	} FunctorFindChar;

	typedef std::list<CharWeight> AosCharsetWeightList;

private:
	//
	// charset and the weight of each char, 
	// the default weight is eCharsetRSGDefaultWeight
	//
	AosCharsetWeightList mCharWeightList;

	//
	// the min length of the string
	//
	u32					mMinLen;

	//
	// the max length of the string
	//
	u32 				mMaxLen;

	bool				mUnique;
	bool				mOrdered;

	u32 				mTotalWeight;

public:
	AosCharsetRSG();
	~AosCharsetRSG();

	virtual bool nextValue(AosValue& value);
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool createIncorrectRVGs(AosRVGPtrArray& rvgList);

	//
	// get a charset string or all charset and weight
	//
	void getCharset(OmnString &str);
	void getCharset(AosCharsetWeightList& charweight);
	
	//
	// add new char and its weight, if the charactor exists then modify its weight
	//
	void addChar(const char c, const int weight=eCharsetRSGDefaultWeight);
	void addCharset(const OmnString & str, const int weight=eCharsetRSGDefaultWeight);
//	void addCharset(const AosCharsetWeightList& charweight);

	//
	// delete one or more charactors
	//
	bool delChar(const char c);
	bool delCharset(const OmnString & str);
	bool clearCharset();

	u32 getMinLen();
	u32 getMaxLen();
	bool getUniqueFlag();
	bool getOrderedFlag();

	void setMinLen(const u32 min);
	void setMaxLen(const u32 max);
	void setUniqueFlag(const bool flag);
	void setOrderedFlag(const bool flag);

private:
	bool nextUniqueOrdered(OmnString & destStr, const u32 len);
	bool nextUniqueNotOrdered(OmnString & destStr, const u32 len);
	bool nextMultipleOrdered(OmnString & destStr, const u32 len);
	bool nextMultipleNotOrdered(OmnString & destStr, const u32 len);

	u32 getTotalWeight();
	void resetCreateTimes();

	
};


#endif


