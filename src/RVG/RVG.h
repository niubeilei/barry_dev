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
#ifndef Aos_RVG_RVG_H
#define Aos_RVG_RVG_H

#include "Util/Ptrs.h"
#include "RVG/CommonTypes.h"
#include "RVG/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlLib/tinyxml.h"

typedef std::list<AosRVGPtr> AosRVGPtrList;
typedef std::vector<AosRVGPtr> AosRVGPtrArray;
//
// Random Value Generator base class
//
class AosRVG : virtual public OmnRCObject 
{
public:
	enum
	{
		eRVGDefaultComposeWeight = 10,
		eRVGDefaultWeight = 10
	};

protected:
	OmnString			mName;
	AosValueType::E		mValueType; 
	AosRVGType::E		mRVGType; 
	OmnString			mProductName;
	//
	// The weight to select this RVG
	//
	u32					mWeight;
	//
	// this weight is used by composeRSG to calculate the percentage of each BRVG
	// when the compose type is "or".
	//
	u32					mComposeWeight;
	//
	// when the compose type is "and", we can use this value to control the repeated times 
	// of this RVG in the dest generated string.
	//
	u32					mComposeMinNum;
	u32					mComposeMaxNum;


public:
	AosRVG();

	virtual ~AosRVG();
	
	// 
	// Chen Ding:
	// This function requires more parameters. Please look at the
	// class I have implemented for strings. 
	// Also, the function should return boolean to indicate whether
	// it generated a value or not. 
	// 
/*	virtual bool nextValue(AosValue &value, 
							const AosVarContainerPtr &data,
							const int correctFlag,
							const AosGenRecordPtr &object, 
							const int selectFromObjFlag,
							bool &isValueCorrect, 
							OmnString &errmsg){return false;};
*/
	virtual bool nextValue(AosValue& value) = 0;
	//
	// Create Incorrect RVGs
	//
	virtual bool createIncorrectRVGs(AosRVGPtrArray& rvgList) = 0;

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual bool writeToFile();
	virtual bool writeToFile(const OmnString &configFile);
	virtual bool deleteConfigFile();

	virtual u32 getWeight() const;
	virtual void setWeight(u32 weight);
	
	virtual u32 getComposeWeight() const;
	virtual void setComposeWeight(u32 weight=eRVGDefaultComposeWeight);

	virtual OmnString getName();
	virtual void setName(OmnString &name);

	virtual OmnString getProductName();
	virtual void setProductName(OmnString &name);

	virtual AosValueType::E getValueType();
	virtual void setValueType(AosValueType::E type);

	virtual AosRVGType::E getRVGType();
	virtual void setRVGType(AosRVGType::E type);

public:
	virtual void setCorrectPecentage(float) {}; // set correct pecentage

	//
	// create RVG instance according to the xml node
	//
	static AosRVGPtr RVGFactory(TiXmlNode* node);
	static AosRVGPtr RVGFactory(AosRVGType::E type);
	static AosRVGPtr RVGFactory(const OmnString &configFile);
		
};
#endif

