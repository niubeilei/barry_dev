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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DLLObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DLLDataProcObj.h"
#include "SEInterfaces/DLLActionObj.h"
#include "SEInterfaces/DLLCondObj.h"
#include "SEInterfaces/DataCalcObj.h"
#include "XmlUtil/XmlTag.h"



AosDLLObj::AosDLLObj(
		const Type type, 
		const OmnString &libname,
		const OmnString &method, 
		const OmnString &version)
:
mType(type),
mLibname(libname),
mMethod(method),
mVersion(version)
{
}


AosDLLObj::~AosDLLObj()
{
}


OmnString
AosDLLObj::enumToString(const Type type)
{
	switch (type)
	{
	case eDataProc:
		 return "DataProc";

	case eDataCalc:
		 return "DataCalc";

	case eAction:
		 return "Action";

	case eCondition:
		 return "Condition";

	default:
		 return "Not Defined";
	}
}

AosDLLDataProcObjPtr 
AosDLLObj::convertToDataProc(const AosRundataPtr &rdata)
{
	if (mType != eDataProc)
	{
		AosSetErrorU(rdata, "not_data_proc") << ": " << enumToString(mType);
		return 0;
	}

	return (AosDLLDataProcObj*)this;
}


AosDataCalcObjPtr 
AosDLLObj::convertToDataCalc(const AosRundataPtr &rdata)
{
	if (mType != eDataCalc)
	{
		AosSetErrorU(rdata, "not_data_calculator") << ": " << enumToString(mType);
		return 0;
	}

	return (AosDataCalcObj*)this;
}


AosDLLActionObjPtr 
AosDLLObj::convertToAction(const AosRundataPtr &rdata)
{
	if (mType != eAction)
	{
		AosSetErrorU(rdata, "not_action") << ": " << enumToString(mType);
		return 0;
	}

	return (AosDLLActionObj*)this;
}


AosDLLCondObjPtr 
AosDLLObj::convertToCondition(const AosRundataPtr &rdata)
{
	if (mType != eCondition)
	{
		AosSetErrorU(rdata, "not_conditoin") << ": " << enumToString(mType);
		return 0;
	}

	return (AosDLLCondObj*)this;
}

