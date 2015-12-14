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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataRecord directory
#include "Util/DataRecord.h"

#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util/DataRecord.h"
#include "Util/RecordFixBin.h"
#include "Util/RecordVariable.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static AosDataRecord*	sgRecords[AosRecordType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;

AosDataRecord::AosDataRecord(
		const OmnString &name, 
		const AosRecordType::E type, 
		const bool flag)
:
mType(type),
mNumElems(-1),
mDocid(0)
{
	if (flag)
	{
		if (!registerRecord(name, this))
		{
			OmnThrowException("failed create data record");
		}
	}
}


AosDataRecord::~AosDataRecord()
{
}


bool
AosDataRecord::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	mElems.clear();
	mNumElems = 0;

	AosXmlTagPtr records = def->getFirstChild("records");
	if(!records) return true;	

	AosXmlTagPtr elemtag = records->getFirstChild();
	while (elemtag)
	{
		AosDataElemPtr elem = AosDataElem::getElem(elemtag, rdata);
		if (!elem)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << records->toString() << enderr;
			return false;
		}

		if (!elem->isFixed())
		{
			AosSetErrorU(rdata, "elem_not_fixed_len") << ": " << records->toString() << enderr;
			return false;
		}

		mElems.push_back(elem);
		elemtag = records->getNextChild();
	}

	mNumElems = mElems.size();
	return true;
}


AosDataRecordPtr 
AosDataRecord::getRecord(const AosXmlTagPtr &def, AosRundata *rdata)
{
	if(!sgInited) staticInit();
	aos_assert_rr(def, rdata, 0);
	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	AosRecordType::E type = AosRecordType::toEnum(ss);
	aos_assert_rr(AosRecordType::isValid(type), rdata, 0);
	AosDataRecord *record = sgRecords[type];
	if (!record)
	{
		AosSetErrorU(rdata, "data_record_not_registered") << ": " << ss;
		AosLogError(rdata);
		return 0;
	}

	return record->clone(def, rdata);
}


bool
AosDataRecord::staticInit()
{
	if(sgInited)    return true;

	static AosRecordFixBin		lsAosRecordFixBin(true);
	static AosRecordVariable	lsAosRecordVariable(true);

	sgInited = true;
	return true;
}


bool
AosDataRecord::registerRecord(const OmnString &name, AosDataRecord *record)
{
	sgLock.lock();
	if (!AosRecordType::isValid(record->mType))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect id: ";
		errmsg << record->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgRecords[record->mType])
	{
		sgLock.unlock();
		OmnString errmsg = "Already registered: ";
		errmsg << record->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgRecords[record->mType] = record;
	sgLock.unlock();
	return true;
}
#endif
