////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 2015/01/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Synchers/Syncher.h"
#include "Util/Buff.h"
#include "API/AosApi.h" 
#include "AppMgr/App.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/IILMgrObj.h"


AosSyncher::AosSyncher(const int version)
:
AosSyncherObj(version),
mDatasetDef(0),
mDeltasBuff(0)
{
}


AosSyncher::~AosSyncher()
{
}


bool 
AosSyncher::proc()
{
	OmnShouldNeverComeHere;
	return false;
}


AosBuffPtr 
AosSyncher::serializeToBuff()
{
	OmnShouldNeverComeHere;
	return NULL;
}


bool 
AosSyncher::serializeFromBuff(const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


AosDatasetObjPtr 
AosSyncher::getSyncherDataset(AosRundata *rdata,
		const AosXmlTagPtr &dataset_def, 
		const AosBuffPtr &buff)
{
	aos_assert_r(mDatasetDef && mDeltasBuff, NULL);

	AosDatasetObjPtr ds = AosCreateDataset(rdata, dataset_def);
	aos_assert_r(ds, NULL);
	ds->setData(rdata, buff);

	return ds;
}


AosSyncherObjPtr
AosSyncher::clone() const 
{
	OmnShouldNeverComeHere;
	return NULL;
}


AosDeltaBeanOpr::E
AosSyncher::getDeltaBeanOpr(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	aos_assert_r(record, AosDeltaBeanOpr::eInvalid);

	OmnString errmsg;
	AosExprObjPtr oprExpr = AosParseExpr("zky_operator;", errmsg, rdata);
	aos_assert_r(oprExpr, AosDeltaBeanOpr::eInvalid);

	AosValueRslt value;
	bool rslt = oprExpr->getValue(rdata, record, value);
	aos_assert_r(rslt, AosDeltaBeanOpr::eInvalid);
	return AosDeltaBeanOpr::toEnum(value.getStr());
}


bool
AosSyncher::appendRecord(AosRundata *rdata, AosDataRecordObj *record)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosSyncher::config(AosRundata *rdata, const AosXmlTagPtr &def)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosSyncher::procData(AosRundata *rdata, AosDataRecordObj *record)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosSyncher::flushDeltaBeans(AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosIILType 
AosSyncher::getIILType(const AosDataType::E type)
{
	if (AosDataType::isNumericType(type))
	{
		return eAosIILType_U64;
	}
	else if (AosDataType::isCharacterType(type))
	{
		return eAosIILType_Str;
	}
	else
	{
		OmnAlarm << "type: " << AosDataType::getTypeStr(type) << enderr;
	}
	
	return eAosIILType_Invalid;
}

