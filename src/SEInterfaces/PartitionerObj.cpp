////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/PartitionerObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"


extern AosPartitionerObjPtr sgPartitioner[AosPartitionerType::eMax+1];
static OmnMutex    	 		sgLock;


AosPartitionerObj::AosPartitionerObj(
		const OmnString &name, 
		const AosPartitionerType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
	AosPartitionerObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerPartitioner(thisptr);
	}
}

AosPartitionerObj::~AosPartitionerObj()
{
}


bool
AosPartitionerObj::registerPartitioner(const AosPartitionerObjPtr &partitioner)
{
	AosPartitionerType::E type = partitioner->mType;
	if (type <= AosPartitionerType::eInvalid || type >= AosPartitionerType::eMax)
	{
		OmnAlarm << "Invalid action id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	bool rslt = AosPartitionerType::addName(partitioner->mName, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!sgPartitioner[type])
	{
		sgPartitioner[type] = partitioner;
	}
	return true;
}


AosPartitionerObjPtr
AosPartitionerObj::getPartitionerStatic(const AosXmlTagPtr &item, const AosRundataPtr &rdata) 
{
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	aos_assert_rr(id != "", rdata, 0);
	
	AosPartitionerType::E type = AosPartitionerType::toEnum(id);
	aos_assert_rr(AosPartitionerType::isValid(type), rdata, 0);
	
	return sgPartitioner[type]->clone(item, rdata);
}


AosPartitionerObjPtr
AosPartitionerObj::getPartitionerStatic(const AosPartitionerType::E type, const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosPartitionerType::isValid(type), rdata, 0);
	return sgPartitioner[type];	
}

