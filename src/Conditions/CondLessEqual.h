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
// "LessEqual" condition is used to check whether the generated value
// is Less or Equal than the specified value. The specified value can get
// from any kinds of way, such as a data buffer or variable. 
//   
//
// Modification History:
// Monday, December 17, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Conditions_CondLessEqual_H
#define Aos_Conditions_CondLessEqual_H

#include "Conditions/CondArith.h"

class AosCondLessEqual : public AosCondArith
{
	OmnDefineRCObject;

public:
	AosCondLessEqual();
	~AosCondLessEqual();

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual bool check(const AosExeDataPtr &ed);
	virtual bool check(const AosValue& value);
	virtual bool check(const AosValue& valueLeft, const AosValue& valueRight);

};


#endif


