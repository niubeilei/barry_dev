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
// 2013/12/31 Created by Phill
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ValueObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"

AosValueObj::AosValueObj(
		const AosDataType::E type, 
		const int version)
:
AosJimo(AosJimoType::eValue, version),
mIsNull(true),
mDataType(type)
{
}


AosValueObj::~AosValueObj()
{
}


AosValueObjPtr
AosValueObj::createValue(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo) return 0;

	if (jimo->getJimoType() != AosJimoType::eValue)
	{
		AosSetErrorUser(rdata, "value_invalid_jimo_type") 
			<< jimo->getJimoType() << enderr;
		return 0;
	}

	AosValueObjPtr value = dynamic_cast<AosValueObj*>(jimo.getPtr());
	if (!value)
	{
		AosSetErrorUser(rdata, "value_internal_error") << enderr;
		return 0;
	}

	return value;
}
