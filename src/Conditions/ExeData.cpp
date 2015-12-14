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
// 01/28/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conditions/ExeData.h"

#include "alarm/Alarm.h"
#include "RVG/Table.h"
#include "SemanticData/Var.h"
#include "Torturer/Command.h"


AosExeData::AosExeData(const AosCommandPtr &cmd)
:
mCommand(cmd)
{
	aos_assert(mCommand);
}


AosParmPtr	
AosExeData::getParm(const std::string &parmName)
{
	aos_assert_r(mCommand, 0);
	return mCommand->getParm(parmName);
}


AosVarPtr	
AosExeData::getVar(const std::string &name)
{
	aos_assert_r(mCommand, 0);
	return mCommand->getVar(name);
}


AosTablePtr	
AosExeData::getTable(const std::string &name)
{
	aos_assert_r(mCommand, 0);
	return mCommand->getTable(name);
}


std::string	
AosExeData::getCommandName() const
{
	aos_assert_r(mCommand, "Invalid");
	return mCommand->getName();
}


AosValuePtr
AosExeData::getValue(const std::string &name) 
{
	aos_assert_r(mCommand, 0);
	AosVarPtr var = getVar(name);
	if (!var) return 0;
	return var->getValue();
}



