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

#ifndef Aos_Conditions_CondDataConst_H
#define Aos_Conditions_CondDataConst_H

#include "Conditions/CondData.h"
#include "RVG/CommonTypes.h"

class AosCondDataConst : public AosCondData
{
private:
	AosValuePtr	mValue;

public:
	AosCondDataConst();
	~AosCondDataConst();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual bool getData(AosValuePtr& value, const AosExeDataPtr &ed);
	void setData(const AosValue& value);

};

#endif
