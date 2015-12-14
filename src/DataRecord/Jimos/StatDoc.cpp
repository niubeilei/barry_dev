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
// A stat doc consists of one root stat doc and zero or more extension
// stat docs.
//
// Root Stat Doc:
// 		(Index Portion)
// 		[time_value(u32), docid]
// 		[time_value(u32), docid]
// 		...
// 		(One Time Bucket)
// 		[value]
// 		[value]
// 		...
//
// Extension Stat Doc (one for each time bucket):
// 		[value]
// 		[value]
// 		...
// 		[value]
//
// The root stat doc consists of two portions: Index and one time bucket. 
// The total number
// of index entries is pre-configured and cannot be changed. The size of 
// one time bucket block is fixed size and can never be changed. This means
// that once created, the size of the root stat doc and the extension
// stat docs can never be changed.
//
// Each stat doc saves only one time series value.
//
// Modification History:
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataRecord/Jimos/StatDoc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

static AosJimoUtil::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosStatDoc_0(
		const AosRundataPtr &rdata, 
		const u32 version) 
{
	try
	{
		return OmnNew AosStatDoc(version);
	}

	catch (...)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosStatDoc::AosStatDoc(const OmnString &version)
:
AosDataRecord(...)
{
}


AosStatDoc::~AosStatDoc()
{
}


AosJimoPtr
AosStatDoc::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const            
{
	try
	{
		return OmnNew AosStatDoc(*this);
	}

	catch (...)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}
	return 0;
}


bool
AosStatDoc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	aos_assert_r(def, false);

	return true;
}



AosDataRecordObjPtr 
AosStatDoc::clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosStatDoc(*this);
	}

	catch (...)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}
}


bool		
AosStatDoc::getFieldValue(
		const AosRundataPtr &rdata,
		AosDataStruct *doc,
		const u32 time,
		AosValueRslt &value)
{
	if (time > 0) return getFieldValue(rdata, doc, time-1, time, value);
	return getFieldValue(rdata, doc, 0, time, value);
}


bool		
AosStatDoc::getFieldValue(
		const AosRundataPtr &rdata,
		AosDataStruct *doc,
		const u32 start,
		const u32 end,
		AosValueRslt &value)
{
	// Values are stored as totals. This function retrieves the value:
	// 		value = value[end] - value[start-1]
	//
	// 'doc' is the root stat doc. It checks whether the time is 
	// valid. If not, a null is returned. Otherwise, it checks
	// whether the value is in the root doc. If yes, the value is retrieved.
	// Otherwise, the value resides in an extension doc. It retrieves
	// that extension doc and then the value.
	OmnNotImplementedYet;
	return false;
}


bool		
AosStatDoc::addValue(
		AosRundata *rdata,
		const u64 time, 
		const i64 value)
{
	// This function checks whether the time is in the root doc or 
	// an extension doc. If needed, it will create the extension doc.
	// It then adds 'value' to the corresponding value.
	OmnNotImplementedYet;
	return false;
}


void
AosStatDoc::clear()
{
}


bool 		
AosStatDoc::setData(
		AosRundata *rdata, 
		char *data,
		const int len,
		const i64 offset,
		const AosBuffDataPtr &metaData)
{
}


AosXmlTagPtr
AosStatDoc::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << mName << ">";
	
	bool rslt = true;
	int idx = 0;
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFields[i]->serializeToXmlDoc(docstr, idx, data, data_len, rdata);
		aos_assert_r(rslt, 0);
	}
	
	docstr << "</" << mName << ">";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	return doc;
}


AosBuffPtr
AosStatDoc::serializeToBuff(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, 0);
		
	bool rslt = true;
	AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFields[i]->serializeToBuff(doc, buff, rdata);
		aos_assert_r(rslt, 0);
	}
	return buff;	
}
#endif
