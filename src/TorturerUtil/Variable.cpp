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
// Modification History:
// 07/27/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerUtil/Variable.h"

#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"


AosVariable::AosVariable()
{
}


AosVariable::~AosVariable()
{
}


bool
AosVariable::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<Global>
	// 		<Name>
	// 		<Value>
	// 	</Global>	
	// 
	OmnXmlGetStr(mName, def, "Name", "", false);
	mValue = def->getStr("Value", "");
	return true;
}


AosVariablePtr
AosVariable::createVariable(const OmnXmlItemPtr &def)
{
	AosVariablePtr var = OmnNew AosVariable();
	if (!var->config(def)) return 0;

	return var;
}


bool
AosVariable::setValue(const OmnString &value)
{
	mValue = value;
	return true;
}

