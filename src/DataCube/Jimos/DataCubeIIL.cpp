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
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeIIL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/DiskStat.h"
#include "DbQuery/Query.h"
#include "Rundata/Rundata.h"
#include "TransBasic/Trans.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "TaskMgr/Task.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeIIL_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeIIL(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
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



AosDataCubeIIL::AosDataCubeIIL(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_IIL, version),
mRundata(0),
//mTotalEntries(0),
mTotalReadEntries(0),
mBlockSize(0),
mIILName(""),
mCaller(0),
mQueryContext(0),
mSnapId(0)
{
}


AosDataCubeIIL::~AosDataCubeIIL()
{
}


bool
AosDataCubeIIL::config(const AosRundataPtr &rdata,	const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mIILName = worker_doc->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);

	mWithKey = worker_doc->getAttrBool("zky_with_key", true);

	mSnapId = 0;
	//Jozhi IIL Query Snapshot
	/*
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	map<u32, AosTaskDataObjPtr> iil_snapshots;
	task->getIILSnapShots(iil_snapshots);
	map<u32, AosTaskDataObjPtr>::iterator itr = iil_snapshots.begin();
	while(itr != iil_snapshots.end())
	{
		set<OmnString> iilnames = (itr->second)->getIILNames();
		if (iilnames.find(mIILName) != iilnames.end())
		{
			mSnapId = (itr->second)->getSnapShotId();
			break;
		}
		itr++;
	}
	*/
	// Ketty 2014/09/17
	/*
	aos_assert_r(mTask, false);
	map<OmnString, AosTaskDataObjPtr> snap_ids;
	mTask->getJobSnapShots(snap_ids);
	if (snap_ids.size() != 0)
	{
		map<OmnString, AosTaskDataObjPtr>::iterator itr = snap_ids.begin();	
		while(itr != snap_ids.end())
		{
			AosTaskDataObjPtr snapshot = itr->second;
			//if (snapshot->getTaskDataType() == AosTaskDataType::eIILSnapShot
			//		&& snapshot->getIILName() == mIILName)
			//{
			//
			//	mSnapId = snapshot->getSnapShotId();
			//	break;
			//}
			++itr;
		}
	}
	*/

	mBlockSize = worker_doc->getAttrU32("zky_blocksize", eDftBlockSize);

	//mTotalEntries = AosIILClient::getSelf()->getTotalNumDocs(mIILName, mSnapId, rdata);
	//if (mBlockSize > mTotalEntries)
	//{
	//	mBlockSize = mTotalEntries;
	//}

	AosXmlTagPtr query_cond = worker_doc->getFirstChild("query_cond");
	aos_assert_r(query_cond, false);

	AosOpr opr = AosOpr_toEnum(query_cond->getAttrStr("zky_opr"));
	if (!AosOpr_valid(opr))
	{
		rdata->setError() << "Operator is invalid: " << opr;        
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool reverse = query_cond->getAttrBool("zky_reverse", false);

	OmnString value = query_cond->getAttrStr("zky_value");
	if (value == "")
	{
		value = query_cond->getNodeText("zky_value");
	}
	if (value == "*")
	{
		opr = eAosOpr_an;
	}

	OmnString value2 = query_cond->getAttrStr("zky_value2");
	if (value2 == "")
	{
		value2 = query_cond->getNodeText("zky_value2");
	}

	mQueryContext = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(mQueryContext, false);
	
	mQueryContext->setOpr(opr);
	mQueryContext->setStrValue(value);                       
	mQueryContext->setReverse(reverse);
	mQueryContext->setBlockSize(mBlockSize);

	if (value2 != "")
	{
		mQueryContext->setStrValue2(value2);
	}

	return true;
}


bool
AosDataCubeIIL::readData(
		const u64 reqid,
		AosRundata *rdata)
{
//OmnScreen << "AosDataCubeIIL AosDataCubeIIL readData reqId:" << reqid << endl;
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);
	AosAsyncRespCallerPtr thisPtr(this, false);
	bool rslt = AosQueryColumnAsync(mIILName, query_rslt, 0, 
			mQueryContext, thisPtr, reqid, mSnapId, rdata);
	aos_assert_r(rslt, false);
	return true;

} 

int 
AosDataCubeIIL::getPhysicalId() const
{
	OmnShouldNeverComeHere;
	return -1;
}


int64_t
AosDataCubeIIL::getFileLength() const
{
	OmnShouldNeverComeHere;
	return -1;
}


OmnString
AosDataCubeIIL::getFileName() const
{
	OmnShouldNeverComeHere;
	return "";
}


AosDataConnectorObjPtr 
AosDataCubeIIL::cloneDataConnector()
{
	return OmnNew AosDataCubeIIL(*this);
}


bool
AosDataCubeIIL::readBlock(
		AosRundata *rdata,
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCubeIIL::appendBlock(
		AosRundata *rdata, 
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeIIL::writeBlock(
		AosRundata *rdata, 
		const int64_t pos,
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeIIL::copyData(
		AosRundata *rdata, 
		const OmnString &from_name,
		const OmnString &to_name)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeIIL::removeData(
		AosRundata *rdata, 
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataCubeIIL::serializeTo(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataCubeIIL::serializeFrom(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr 
AosDataCubeIIL::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeIIL(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


void 
AosDataCubeIIL::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;	
}


void
AosDataCubeIIL::callback(
	const AosTransPtr &trans,
	const AosBuffPtr &resp,
	const bool svr_death)
{
	if(svr_death)
	{
		OmnScreen << "send Trans failed. svr death."
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; death_svr_id:" << trans->getToSvrId()
			<< endl;
		return;
	}
	
	aos_assert(resp);
	bool rslt = resp->getU8(0);
	u64 reqId = resp->getU64(0);
	if(!rslt)
	{
		mCaller->callBack(reqId, 0, true);
		return;
	}
//OmnScreen << "AosDataCubeIIL AosDataCubeIIL AosDataCubeIIL reqId: " << reqId << endl;
	
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert(query_rslt);

	AosXmlTagPtr xml = getXmlFromBuff(resp, mRundata.getPtr());
	aos_assert(xml);
	rslt = mQueryContext->serializeFromXml(xml, mRundata);
	aos_assert(rslt);

	xml = getXmlFromBuff(resp, mRundata.getPtr());
	aos_assert(xml);
	rslt = query_rslt->serializeFromXml(xml);
	aos_assert(rslt);

	u32 numdocs = query_rslt->getNumDocs();
//OmnScreen << "AosDataCubeIIL AosDataCubeIIL AosDataCubeIIL reqId:" << reqId << ";total:" << numdocs << ";totalEntries:" << mTotalEntries << ";"<< endl;
OmnScreen << "AosDataCubeIIL AosDataCubeIIL AosDataCubeIIL reqId:" << reqId << ";total:" << numdocs << ";" << endl;

	u64 value;
	OmnString key;
	u64	u64_key;
	u32 totaldocs = 0;
	bool finished = false;
	AosBuffPtr buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	while(1)
	{
		if (!mWithKey)
		{
			rslt = query_rslt->nextDocidValue(value, u64_key, finished, mRundata);
			aos_assert(rslt);
			if (finished) break;
			buff->setU64(value);
		}
		else
		{
			rslt = query_rslt->nextDocidValue(value, key, finished, mRundata);
			aos_assert(rslt);
			if (finished) break;
			aos_assert((int)strlen(key.data()) == key.length());
			buff->setOmnStr(key);
			buff->setU64(value);
		}
		totaldocs++;
	}
	aos_assert(totaldocs == numdocs);
	mTotalReadEntries += numdocs;
	//aos_assert(mTotalReadEntries <= mTotalEntries);

	bool isfinished = false;
	//if (mTotalReadEntries == mTotalEntries) 
	if (mQueryContext->finished())
	{
		isfinished = true;
	}

	buff->reset();
	AosDiskStat disk_stat;
	AosBuffDataPtr buff_data = OmnNew AosBuffData(0, buff, disk_stat);
	aos_assert(mCaller);
	mCaller->callBack(reqId, buff_data, isfinished);
}


AosXmlTagPtr
AosDataCubeIIL::getXmlFromBuff(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(buff, 0);
	
	u32 len = buff->getU32(0);
	aos_assert_r(len, 0);

	AosBuffPtr b = buff->getBuff(len, false AosMemoryCheckerArgs);
	aos_assert_r(b, 0);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(b->data(), len, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, 0);

	return xml;
}




