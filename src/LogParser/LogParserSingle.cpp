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
// This parser assumes the log uses just one type of records. This parser
// provides individual log entry parsing. If we need to scan a log, please
// use dataset. 
//
// Modification History:
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogParserSingle/LogParserSingle.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosLogParserSingle_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosLogParserSingle(version);
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


AosLogParserSingle::AosLogParserSingle(const int version)
:
AosLogParserSingleObj(version),
mLock(OmnNew OmnMutex())
{
}


AosLogParserSingle::~AosLogParserSingle()
{
}


bool 
AosLogParserSingle::config(	
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	AosXmlTagPtr record_def;
	if (worker_doc)
	{
		//  <worker_doc ...>
		// 		<record_def .../>		
		//  </worker_doc ...>
		record_def = worker_doc->getFirstChild("record_def");
	}

	if (!record_def && jimo_doc)
	{
		record_def = jimo_doc->getFirstChild("record_def");
	}

	if (!record_def)
	{
		AosLogError(rdata, AOSERR_USER_ERROR, "missing_record_def")
			<< AosPair("worker_doc", worker_doc)
			<< AosPair("jimo_doc", jimo_doc) << enderr;
		return false;
	}

	mRecord = AosDataRecordObj::createDataRecordStatic(record_def, rdata);
	aos_assert_rr(mRecord, rdata, false);
	mRecordRaw = mRecord.getPtr();

	return true;
}


bool
AosLogParserSingle::parseEntry(
		const char *data, 
		const int data_len, 
		const int offset,
		AosBuffData *metadata,
		AosDataRecordObjPtr &record,
		AosRundata *rdata)
{
	// This function parses one log entry. The log entry is in 'data'
	// starting at the position 'offset'. 'metadata' contains the 
	// information about where 'data' was read (if it was read from
	// a file). 
	//
	// This function assumes that the caller wants to read individual
	// log entries. If one needs to read a large amount of log entries
	// (i.e., scanning a log), one should use datasets. 
	//
	// This function should always return true.
	
	aos_assert_rr(data, rdata, false);
	aos_assert_rr(mRecordRaw, rdata, false);

	record = mRecordRaw->clone();
	bool rslt = record->setData(data, data_len, metadata, offset);
	if (!rslt) return false;
	return true;
}

