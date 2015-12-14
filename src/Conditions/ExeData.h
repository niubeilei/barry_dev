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
// 01/25/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_ExeData_H
#define Aos_Conditions_ExeData_H

#include "RVG/Ptrs.h"
#include "SemanticData/Ptrs.h"
#include "Torturer/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <string>

class AosExeData : virtual public OmnRCObject
{
	AosCommandPtr		mCommand;

protected:

public:
	AosExeData(const AosCommandPtr &cmd);
	virtual ~AosExeData(){};

	AosParmPtr	getParm(const std::string &parmName);
	AosVarPtr	getVar(const std::string &name);
	AosValuePtr	getValue(const std::string &name);
	AosTablePtr	getTable(const std::string &name);
	std::string	getCommandName() const;
};

#endif
