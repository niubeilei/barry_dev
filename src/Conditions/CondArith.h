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
// December 28, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Conditions_CondArith_H
#define Aos_Conditions_CondArith_H

#include "Conditions/Condition.h"

class AosCondArith : public AosCondition
{
protected:
	AosCondDataPtr mLeftValue;
	AosCondDataPtr mRightValue;

public:
	AosCondArith();
	~AosCondArith();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	
	void setLeftValue(const AosCondDataPtr& condData);
	void setRightValue(const AosCondDataPtr& condData);
	void getLeftValue(AosCondDataPtr& condData);
	void getRightValue(AosCondDataPtr& condData);

};


#endif


