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
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef  Aos_RVG_ENUM_RSG_H
#define  Aos_RVG_ENUM_RSG_H

#include "RVG/RSG.h"
#include "RVG/RIG.h"
#include "Util/RCObject.h"
#include <functional>


typedef std::list<OmnString> AosEnumStrList;
//
// generate a string from the list randomly
//
class AosEnumRSG : public AosRSG
{
	OmnDefineRCObject;
public:
	enum
	{
		eEnumRSGDefaultWeight = 100
	};

	typedef struct StrWeight
	{
		OmnString enumStr;
		u32 weight;
		u32 createTimes;
	} StrWeight;

	//
	// a functor to find enum string, used by find/remove agorithm
	//
	typedef struct FunctorFindEnumStr : public std::binary_function<StrWeight, OmnString , bool>
	{
		bool operator()(const StrWeight& strWeight, const OmnString &str) const
		{
			return (strWeight.enumStr == str);
		}
		
	} FunctorFindEnumStr;

	typedef std::vector<StrWeight> AosEnumStrWeightList;

protected:
	AosEnumStrWeightList 	mEnumStrWeight;
	u32						mTotalWeight;
	AosRIG<u32>				mWeightHash;

public:
	AosEnumRSG();
	virtual ~AosEnumRSG();

	virtual bool nextValue(AosValue& value);

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool createIncorrectRVGs(AosRVGPtrArray& rvgList);
	//
	// get enumlated string list, or all string and weight list
	//
	void getEnumString(AosEnumStrList& strList);
	void getEnumString(AosEnumStrWeightList& strWeightList);

	//
	// add an enumlated string, if exist it will replace the old string weight pair
	//
	void setEnumString(const OmnString &str, u32 weight=eEnumRSGDefaultWeight);
//	void addEnumString(const AosEnumStrWeightList& strWeightList);

	//
	// delete enumerated string
	//
	bool removeEnumString(const OmnString &str);
//	bool removeEnumString(const AosEnumStrList& strList);
	void clearEnumString();
	
protected:
	u32 getTotalWeight(AosEnumStrWeightList& strWeightList);
	void resetCreateTimes();
	void reconstructWeightHash();

};

#endif
