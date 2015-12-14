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
// This is management the Log
//
// Modification History:
// 06/23/2011 Create by Tom 
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/LogAging.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/File.h"
#include "ValueSel/ValueRslt.h"
#include "Rundata/Ptrs.h"
#include "Util/StrSplit.h"
#include "SeLogSvr/LogCapAging.h"


const OmnString sgDftAgingType = AosSeLogAgingCap;

AosLogAging::AosLogAging(const AgingType type)
:
mAgingType(type)
{
}


AosLogAging::~AosLogAging()
{
}


AosLogAgingPtr
AosLogAging::getAging(AosBuff &buff)
{
	AgingType type = (AgingType)buff.getChar(eInvalidAging);
	aos_assert_r(type != eInvalidAging, 0);
	switch (type)
	{
	case eCapacityAging:
		 return OmnNew AosLogCapAging(buff);

	default:
		 break;
	}

	OmnAlarm << ". Invalid aging type: " << type << enderr;
	return 0;
}


AosLogAgingPtr
AosLogAging::getAging(
		const AosXmlTagPtr &config,
		const AosRundataPtr &rdata)
{
	AgingType type = eCapacityAging;
	if (config)
	{
		type = agingToEnum(config->getAttrStr(AOSTAG_AGING_TYPE, AosSeLogAgingCap), rdata);
	}

	switch(type)
	{
	case eCapacityAging:
		 return OmnNew AosLogCapAging(config, rdata);

	default:
		 break;
	}

	AosSetError(rdata, AosErrmsgId::eInvalidAging);
	OmnAlarm << rdata->getErrmsg() << ". Invalid aging type: " << type << enderr;
	return NULL;
}


AosLogAging::AgingType
AosLogAging::agingToEnum(const OmnString &type, const AosRundataPtr &rdata)
{
	const char* data = type.data();
	int len = type.length();
	if(len<=0) return eInvalidAging;

	switch(data[3])
	{
	case 'c':
		 if(type == AosSeLogAgingCap) return eCapacityAging;
		 break;

	case 'r':
		 if(type == AosSeLogAgingRcd) return eNumRecordsAging;
		 break;

	default:
		 break;
	}

	AosSetError(rdata, AosErrmsgId::eInvalidAging);
	OmnAlarm << rdata->getErrmsg() << ". Aging: " << type << enderr;
	return eInvalidAging;
}


