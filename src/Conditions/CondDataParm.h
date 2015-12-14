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
// This class wraps the last generated value of the parameter
//   
//
// Modification History:
// Wednesday, January 02, 2008: Created by Allen Xu 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_CondDataParm_H
#define Aos_Conditions_CondDataParm_H

#include "Conditions/CondData.h"
#include "RVG/CommonTypes.h"

class AosCondDataParm : public AosCondData
{
private:
	AosParmPtr mParm;
	std::string mParmName;

public:
	AosCondDataParm();
	~AosCondDataParm();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual bool getData(AosValuePtr& value, const AosExeDataPtr &ed);

	std::string getParmName();
	void setParmName(const std::string& name);

};

#endif
