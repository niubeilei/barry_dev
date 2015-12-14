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
// Wednesday, January 02, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Conditions_CondData_H
#define Aos_Conditions_CondData_H

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "RVG/Ptrs.h"
#include "Conditions/CondUtil.h"
#include "Conditions/Ptrs.h"
#include "XmlLib/tinyxml.h"
#include <string>

class AosCondData : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	std::string 		mDataFlag;
	AosCondDataType::E	mCondDataType;

public:
	AosCondData(){};
	virtual ~AosCondData(){};
	virtual bool serialize(TiXmlNode& node) = 0;
	virtual bool deserialize(TiXmlNode* node) = 0;

	virtual bool getData(AosValuePtr& value, const AosExeDataPtr &ed) = 0;
	virtual std::string getDataFlag(){return mDataFlag;};

	virtual AosCondDataType::E getDataType() {return mCondDataType;};


	static AosCondDataPtr factory(TiXmlNode* node);
	static AosCondDataPtr factory(AosCondDataType::E condDataType);

};

#endif
