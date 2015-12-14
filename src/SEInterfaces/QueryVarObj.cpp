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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryVarObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryFuncObj.h"

AosQueryVarObjPtr AosQueryVarObj::smObject;

AosQueryVarObj::AosQueryVarObj(const int version)
:
AosJimo(AosJimoType::eQueryVar, version)
{
}


AosQueryVarObj::AosQueryVarObj()
:
AosJimo(AosJimoType::eQueryVar, 0)
{
}



AosQueryVarObj::~AosQueryVarObj()
{
}


bool 
AosQueryVarObj::evalStatic(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	aos_assert_rr(smObject, rdata, 0);

	aos_assert_rr(def, rdata, false);
	OmnString var_type = def->getAttrStr("var_type");
	if (var_type == AOSVALUE_LITERAL)
	{
		value.setStr(def->getNodeText());
		return true;
	}

	if (var_type == AOSVALUE_FUNC)
	{
		return AosQueryFuncObj::evalStatic(rdata, def, value);
	}

	if (var_type != AOSVALUE_VAR)
	{
		AosSetError(rdata, "queryvarmgr_invalid_var_type")
			<< def << enderr;
		return false;
	}
	return smObject->pickJimo(rdata, def, value);
}


AosQueryVarObjPtr
AosQueryVarObj::createStaticQueryVarObj()
{
	return smObject;
}

AosJimoPtr 
AosQueryVarObj::cloneJimo() const
{
	return 0;
}
