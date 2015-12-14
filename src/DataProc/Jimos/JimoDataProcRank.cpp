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
// 2013/07/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataProc/Jimos/JimoDataProcRank.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ConditionObj.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosJimoDataProcRank_0(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoDataProcRank(rdata, jimo_doc, version);
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


AosJimoDataProcRank::AosJimoDataProcRank(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version)
:
AosJimo(rdata, jimo_doc),
mVersion(version),
mJimoName("dataproc_rank")
{
	if (!init(rdata, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosJimoDataProcRank::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool
AosJimoDataProcRank::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	// registerMethod("next_value", sgFuncMap, 
	// 		AosMethodId::eAosRundata_Jimo_WorkerDoc,
	// 		(void *)AosJimoDataProcRank_nextValue);

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosJimoDataProcRank::AosJimoDataProcRank(
		const AosJimoDataProcRank &rhs)
:
AosJimo(rhs),
mVersion(rhs.mVersion)
{
}

AosJimoDataProcRank::~AosJimoDataProcRank()
{
}


bool
AosJimoDataProcRank::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker)
{
	/*
	aos_assert_r(worker, false);

	mNeedConvert = worker->getAttrBool(AOSTAG_NEED_CONVERT, false);
	mDataColId = worker->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	mKeyFieldName = worker->getAttrStr(AOSTAG_KEYFIELDNAME);
	aos_assert_r(mKeyFieldName != "", false);
	
	mNeedDocid = true;
	mIILType = AosIILType_toCode(worker->getAttrStr(AOSTAG_IILTYPE));
	switch (mIILType)
	{
	case eAosIILType_U64:
	case eAosIILType_Str:
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_iiltype:") << worker->toString();
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}
	
	AosXmlTagPtr tag = worker->getFirstChild(AOSTAG_FILTER);
	if (tag)
	{
		AosCondType::E type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			mFilter = AosConditionObj::getConditionStatic(tag, rdata);
		}
	}

	*/
	return true;
}


OmnString
AosJimoDataProcRank::toString() const
{
	return "";
}


bool 
AosJimoDataProcRank::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoDataProcRank::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoDataProcRank::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosJimoDataProcRank(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool 
AosJimoDataProcRank::run(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


void * 
AosJimoDataProcRank::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}


bool
AosJimoDataProcRank::procData(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	// Refer to ActIILBatchOpr::createIIL(...);
	// When this function is called, there is scanner that holds some data.
	// We need to get the data, which is a buff array. The data is already
	// sorted based on the values. This function loops over all the records. 
	// For each record, it sets the rank. After that, it saves the contents
	// back to the original file. 
	/*
	// This function creates one entry based on 'record'. The inputs
	// can be either an IIL or a record of any format. It assumes 
	// 		[key, field, field, ..., docid]
	// It retrieves only 'key' and 'docid' from 'record':
	// 		[key+docid]
	// It uses this to create a buff array. 
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);
	aos_assert_r(mKeyFieldIdx >= 0, AosDataProcStatus::eError);

	// 1. Retrieve the key
	AosValueRslt key_rslt;
	bool rslt = record->getFieldValue(mKeyFieldIdx, key_rslt, rdata);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	// 2. Filter it as needed.
	if (mFilter && !mFilter->evalCond(key_rslt, rdata))
	{
		return AosDataProcStatus::eContinue;
	}
	
	AosValueRslt value_rslt;
	OmnString vv;
	u64 uu;
	switch (mIILType)
	{
	case eAosIILType_Str:
		 vv = key_rslt.getValueStr1();
		 if(vv == "") return AosDataProcStatus::eContinue;
		 value_rslt.setKeyValue(vv, false, docid);
		 break;

	case eAosIILType_U64:
	 	 rslt = key_rslt.getU64Value(uu, rdata);
	 	 if (!rslt) return AosDataProcStatus::eError;
		 value_rslt.setKeyValue(uu, docid);
		 break;

	default:
		 AosSetErrorU(rdata, "unrecog_iiltype") << ": " << mIILType << enderr;
		 return AosDataProcStatus::eError;
	}
		 
	rslt = mIILAssembler->appendEntry(value_rslt, rdata);
	if (!rslt) return AosDataProcStatus::eError;
	return AosDataProcStatus::eContinue;
	*/
	return true;
}


bool 
AosJimoDataProcRank::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	/*
	bool rslt = AosDataProcObj::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	mKeyFieldIdx = record->getFieldIdx(mKeyFieldName, rdata);
	aos_assert_r(mKeyFieldIdx >= 0, false);
	*/
	return true;
}
#endif
