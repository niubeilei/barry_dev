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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataProc/Jimos/JimoDataProcDataSorter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoDataProcDataSorter_0(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoDataProcDataSorter(rdata, jimo_doc, version);
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


AosJimoDataProcDataSorter::AosJimoDataProcDataSorter(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version)
:
AosJimo(rdata, jimo_doc),
mVersion(version),
mJimoName("dataproc_datasorter")
{
	if (!init(rdata, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosJimoDataProcDataSorter::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool
AosJimoDataProcDataSorter::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	// registerMethod("next_value", sgFuncMap, 
	// 		AosMethodId::eAosRundata_Jimo_WorkerDoc,
	// 		(void *)AosJimoDataProcDataSorter_nextValue);

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosJimoDataProcDataSorter::AosJimoDataProcDataSorter(
		const AosJimoDataProcDataSorter &rhs)
:
AosJimo(rhs),
mVersion(rhs.mVersion)
{
}

AosJimoDataProcDataSorter::~AosJimoDataProcDataSorter()
{
}


bool
AosJimoDataProcDataSorter::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	return true;
}


OmnString
AosJimoDataProcDataSorter::toString() const
{
	return "";
}


bool 
AosJimoDataProcDataSorter::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoDataProcDataSorter::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoDataProcDataSorter::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosJimoDataProcDataSorter(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


AosDataProcObjPtr
AosJimoDataProcDataSorter::clone()
{
	try
	{
		return OmnNew AosJimoDataProcDataSorter(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool 
AosJimoDataProcDataSorter::run(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


void * 
AosJimoDataProcDataSorter::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}


/*
bool
AosDataProcSorter::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


AosDataProcStatus::E 
AosDataProcSorter::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	// AOS_DECLARE_TIMESTAMPS(4, 1000000);
	aos_assert_r(mDataAssembler, AosDataProcStatus::eError);
	aos_assert_r(mKeyFieldIdx >= 0, AosDataProcStatus::eError);

	// 1. Filter it as needed.
	if (mFilter && !mFilter->evalCond(record, rdata))
	{
		return AosDataProcStatus::eContinue;
	}

	if (mConverter)
	{
		bool rslt = mConverter->convertData(rdata, record, output);
		if (!rslt) return AosDataProcStatus::eError;
	}
	
	bool rslt = mDataAssembler->appendEntry(output, rdata);
	if (!rslt) return AosDataProcStatus::eError;
	return AosDataProcStatus::eContinue;
}


AosDataProcObjPtr
AosDataProcSorter::clone()
{
	return OmnNew AosDataProcSorter(*this);
}


AosDataProcObjPtr
AosDataProcSorter::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcSorter * proc = OmnNew AosDataProcSorter(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcSorter::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	return true;
}
*/
#endif
