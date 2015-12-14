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
// 01/28/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_CondDataTable_H
#define Aos_Conditions_CondDataTable_H

#include "Conditions/CondData.h"
#include "RVG/CommonTypes.h"

class AosCondDataTable : public AosCondData
{
private:
	std::string mTableName;

public:
	AosCondDataTable();
	~AosCondDataTable();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual bool getData(AosValuePtr& value, const AosExeDataPtr &ed);
};

#endif
