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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCube/Jimos/DataCubeBuff.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/Task.h"
#include "Debug/Debug.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeBuff_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeBuff(version);
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


AosDataCubeBuff::AosDataCubeBuff(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_BUFF, version)
{
	mDataId = 0;
}


AosDataCubeBuff::~AosDataCubeBuff()
{
}


AosDataConnectorObjPtr 
AosDataCubeBuff::cloneDataConnector()
{
	return OmnNew AosDataCubeBuff(*this);
}


AosJimoPtr 
AosDataCubeBuff::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeBuff(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosDataCubeBuff::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	//  worker_doc format 
	//  <dataconnector jimo_objid="datacube_buff" >
	//	</dataconnector>
	
	OmnTagFuncInfo << endl;
	mDataId = worker_doc->getAttrStr("zky_dataset_name", "");
	return true;
}


bool
AosDataCubeBuff::start(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDataCubeBuff::readData(const u64 reqid, const AosRundataPtr &rdata)
{
	AosBuffDataPtr buff_data = 0;
	AosDiskStat disk_stat;

	//get data from mTask's taskdatabuff
	//AosTaskDataBuff *buffData = ((AosTask*)mTask.getPtr())->getTaskDataBuff(mDataId);   	
	//mBuff = buffData->getNextBuff();
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	mBuff = task->getNextBuff(mDataId);
	if (mBuff.notNull())
	{
		//data is in the mBuff already. 
		buff_data = OmnNew AosBuffData(0, mBuff, disk_stat);
		OmnTagFuncInfo << "Read data from buff cube. data size is: " 
			<< mBuff->dataLen() << endl;
	}

	OmnTagFuncInfo << "Read data from buff cube. DataId is: " << mDataId << endl;
	callBack(reqid, buff_data, true);
	return true;
}


void
AosDataCubeBuff::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff,
		bool finish)
{
	mCaller->callBack(reqid, buff, finish);
}


void
AosDataCubeBuff::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;
}


bool
AosDataCubeBuff::setValueBuff(
			const AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	mBuff = buff;
	mBuff->reset();
	return true;
}

#endif
