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
//	03/29/2013	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "QueryClient/BatchQuery.h"
#include "QueryClient/Ptrs.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"
#include "API/AosApiC.h"
#include "API/AosApiM.h"
#include "API/AosApiR.h"
#include "QueryRslt/QueryRslt.h"
#include "QueryClient/BatchQueryReader.h"
#include "SEInterfaces/BitmapObj.h"
#include "DocClient/DocidShufflerMgr.h"
#include "SEInterfaces/BuffData.h"
#include "DataScanner/DocScanner.h"

AosBatchQuery::AosBatchQuery(
		const OmnString &queryid, 
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata AosMemoryCheckDecl)
:
mBuffSize(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mProcDocsNum(0),
mFinished(false),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mQueryId << queryid << "_QUERYID_";
}


AosBatchQuery::~AosBatchQuery()
{
//	OmnScreen << "destruct : address " << (long)this << endl;
}


bool
AosBatchQuery::addQueryRslt(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &fields,
		vector<AosQrUtil::FieldDef> fielddef,
		const AosQueryRsltObjPtr &queryrslt)
{
	reset(rdata);
	mQueryRslt = queryrslt;
	mFieldDef = fielddef;

	// Error checking
	if(mQueryRslt->getTotal(mRundata) <=0)
	{
		OmnAlarm << "No Data : " << mQueryRslt->getTotal(mRundata) << enderr;
		return false;
	}

	// Constructing the map
	bool finished = false;
	u64 docid=0;
	while((docid = mQueryRslt->nextDocid(finished)) && !finished)
	{
	 	struct AosBatchRecord record;
	 	mDocidMap[docid] = record;
	}

	// Packet docids.
	int total = mQueryRslt->getNumDocs();
	u64 * docids = mQueryRslt->getDocidsPtr();

	//get data
	AosDataScannerObjPtr scanobj = AosDataScannerObj::createDocScannerStatic(
			rdata, docids, total, fields);

	while(1)
	{
		AosBuffDataPtr metaData = OmnNew AosBuffData();
		bool rslt = scanobj->getNextBlock(metaData, rdata);
		aos_assert_r(rslt, false);
		AosBuffPtr buff = metaData->getBuff();
		if (!buff || buff->dataLen() <= 0)
		{
			//finish
			mFinished = true;
			break;
		}
		buff->reset();
		procDocs(buff, rdata);
	}

	createColumns(rdata);

	return true;
}


bool	
AosBatchQuery::procDocs(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	// 'buff' is:
	// 		[idx, docid, field1, field2, ..., fieldn]
	// 		[idx, docid, field1, field2, ..., fieldn]
	// 		...
	// 		[idx, docid, field1, field2, ..., fieldn]
	//
	//record format:
	//1.modify mDocidMap
	//2.check crt_size of AosBlock is equel to block_size 
	//	if crt_size is equal to block_size, save block
	//	if crt_size less than block_size, continue process each record 
	OmnScreen << "---------------- start procdocs ------------------" << endl;


	mBuffSize += buff->dataLen();
	u32 record_start = 0;
	u32 record_length = 0;
	u32 crtidx = 0;
	aos_assert_r(buff, false);
	while(crtidx < buff->dataLen())
	{
		record_start = buff->getCrtIdx();
		record_length = buff->getU32(0);
		if(record_length <= 0)
		{
			OmnAlarm << "Never Failed : record_length <= " << record_length << enderr;
			return false;
		}

		u64 docid = buff->getU64(0);
		crtidx += record_length + sizeof(u32) + sizeof(u64);
		buff->setCrtIdx(crtidx);
		if(mDocidMap[docid].r_isMarked)
		{
			OmnAlarm << "this record is marked: " << docid << enderr;
			continue;
		}
		mDocidMap[docid].r_buff=buff;
		mDocidMap[docid].r_buff_start=record_start;
		mDocidMap[docid].r_buff_len=record_length;
		mDocidMap[docid].r_isMarked = true;
	}
	mProcDocsNum++;
	return true;
}

bool	
AosBatchQuery::reset(const AosRundataPtr &rdata)
{
	mDocidMap.clear();
	mColumns.clear();
	mBuffSize=0;
	mFinished = false;
	return true;
}


bool 
AosBatchQuery::createColumns(const AosRundataPtr &rdata)
{
	//init column buff
	u32 size = mFieldDef.size();
	mColumns.clear();
	for (u32 i=0; i<size; i++)
	{
		AosBuffPtr column = OmnNew AosBuff(AosMemoryCheckerArgsBegin); 
		OmnString column_name = mFieldDef[i].cname;
		mColumns[column_name] = column;
	}
	
	mQueryRslt->reset();
	bool finished = false;
	u64 docid = 0;
	while((docid = mQueryRslt->nextDocid(finished)) && !finished)
	{
		struct AosBatchRecord batchbuff = mDocidMap[docid]; 
		AosBuffPtr buff = batchbuff.r_buff;
		int startidx = batchbuff.r_buff_start;
		int length = batchbuff.r_buff_len;
		buff->setCrtIdx(startidx + sizeof(u32) + sizeof(u64));
		int column_length = 0;
		for(u32 j=0; j<size; j++)
		{
			OmnString column_name = mFieldDef[j].cname;
			AosBuffPtr column = mColumns[column_name];
			OmnString value = buff->getOmnStr("");
			OmnScreen << "field value : " << value << endl;
			column->setOmnStr(value);
			column_length += value.length()+sizeof(int);
		}
		aos_assert_r(length == column_length, false);
	}

	return true;
}
