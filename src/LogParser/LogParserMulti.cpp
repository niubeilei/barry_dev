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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogParserMulti/LogParserMulti.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosLogParserMulti_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosLogParserMulti(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosLogParserMulti::AosLogParserMulti(const int version)
:
AosLogParserMultiObj(version),
mLock(OmnNew OmnMutex())
{
}


AosLogParserMulti::~AosLogParserMulti()
{
}


bool 
AosLogParserMulti::config(	
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	AosXmlTagPtr records_def;
	if (worker_doc)
	{
		//  <worker_doc ...>
		// 		<records>
		// 			<record .../>
		// 			<record .../>
		// 			...
		// 		</records>
		//  </worker_doc ...>
		records_def = worker_doc->getFirstChild("records");
	}

	if (!records_def && jimo_doc)
	{
		records_def = jimo_doc->getFirstChild("records");
	}

	if (!record_def)
	{
		AosLogError(rdata, AOSERR_USER_ERROR, "missing_record_def")
			<< AosPair("worker_doc", worker_doc)
			<< AosPair("jimo_doc", jimo_doc) << enderr;
		return false;
	}

	AosXmlTagPtr rcddef = records_def->getFirstChild();
	while (rcddef)
	{
		int rcd_id = rcddef->getAttrInt("record_id", -1);
		if (rcd_id < 0)
		{
			AosLogError(rdata, AOSERR_USER_ERROR, "invalid_record_id")
				<< AosPair("worker_doc", worker_doc)
				<< AosPair("jimo_doc", jimo_doc) << enderr;
			return false;
		}

		AosDataRecordObjPtr rcd = AosDataRecordObj::createDataRecordStatic(rcddef, rdata);
		aos_assert_rr(rcd, rdata, false);
		mRecords.push_back(rcd);
		mRecordMap[rcd_id] = rcd.getPtr();

		rcddef = records_def->getNextChild();
	}

	if (mRecords.size() <= 0)
	{
		AosLogError(rdata, AOSERR_USER_ERROR, "missing_records")
			<< AosPair("worker_doc", worker_doc)
			<< AosPair("jimo_doc", jimo_doc) << enderr;
	}

	return true;
}


bool
AosLogParserMulti::parseEntry(
		const char *data, 
		const int data_len, 
		const int offset,
		AosBuffData *metadata,
		AosDataRecordObj *&record,
		AosRundata *rdata)
{
	aos_assert_rr(data, rdata, false);
	aos_assert_rr(record, rdata, false);

	int record_id = readRecordId(data, data_len, offset, rdata);
	if (!isValidRecordId(record_id))
	{
		AosLogError(rdata, AOSERRTYPE_DATA_ERROR)
			<< AosPair("record_id") << record_id
			<< AosPair("data_len") << data_len
			<< AosPair("offset") << offset << enderr;
		return false;
	}
	
	record = findRecord(record_id);
	if (!record)
	{
		AosLogError(rdata, AOSERRTYPE_DATA_ERROR)
			<< AosPair("reason") << "record_not_found"
			<< AosPair("record_id") << record_id << enderr;
		return false;
	}

	return true;
}

