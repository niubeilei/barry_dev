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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/TaskData.h"

#include "TaskMgr/TaskDataAuto.h"
#include "TaskMgr/TaskDataDir.h"
#include "TaskMgr/TaskDataFile.h"
#include "TaskMgr/TaskDataNorm.h"
#include "TaskMgr/TaskDataUnicomDir.h"
#include "TaskMgr/TaskDataOutPutIIL.h"
#include "TaskMgr/TaskDataOutPutDoc.h"
#include "TaskMgr/TaskDataOutPutDocid.h"
#include "TaskMgr/TaskDataOutPutSnapShot.h"
#include "TaskMgr/TaskDataIILSnapShot.h"
#include "TaskMgr/TaskDataDocSnapShot.h"
#include "TaskMgr/TaskDataDocIds.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Debug/Debug.h"


static AosTaskDataObjPtr	sgTaskData[AosTaskDataType::eMax];
static OmnMutex				sgLock;
static bool             	sgInited = false;


AosTaskData::AosTaskData(
		const AosTaskDataType::E type, 
		const OmnString &name, 
		const bool flag)
:
mType(type),
mName(name)
{
	if (flag) registerTaskData(type, name, this);
}


AosTaskData::~AosTaskData()
{
}


bool
AosTaskData::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	return true;
}


bool
AosTaskData::staticInit()
{
	if(sgInited)    return true;

	//static AosTaskDataAuto		sgAosTaskDataAuto(true);
	//static AosTaskDataDir		sgAosTaskDataDir(true);
	//static AosTaskDataFile  	sgAosTaskDataFile(true);
	//static AosTaskDataNorm		sgAosTaskDataNorm(true);
	//static AosTaskDataDocIds	sgAosTaskDataDocIds(true);
	//static AosTaskDataUnicomDir sgAosTaskDataUnicomDir(true);

	static AosTaskDataOutPutIIL			sgAosTaskDataOutPutIIL(true);
	//static AosTaskDataOutPutDoc			sgAosTaskDataOutPutDoc(true);
	static AosTaskDataOutPutDocid		sgAosTaskDataOutPutDocid(true);
	static AosTaskDataOutPutSnapShot	sgAosTaskDataOutPutSnapShot(true);
	static AosTaskDataIILSnapShot		sgAosTaskDataIILSnapShot(true);
	static AosTaskDataDocSnapShot		sgAosTaskDataDocSnapShot(true);
	sgInited = true;
	return true;
}


bool
AosTaskData::registerTaskData(
		const AosTaskDataType::E type, 
		const OmnString &name, 
		const AosTaskData *data)
{
	if (!AosTaskDataType::isValid(type))
	{
		OmnAlarm << "Invalid job id: " << type << enderr;
		return false;
	}

	OmnTagFuncInfo << "register task data with name: " << name << endl;
	sgLock.lock();
	if (sgTaskData[type])
	{
		sgLock.unlock();
		OmnAlarm << "Task already registered: " << mName << ":" << type << enderr;
		return false;
	}

	sgTaskData[type] = (AosTaskData *)data;
	OmnString errmsg;
	bool rslt = AosTaskDataType::addName(name, type, errmsg);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add task work data name: " << errmsg << enderr;
	}
	return rslt;
}


AosTaskDataObjPtr 
AosTaskData::createTaskData(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	if(!sgInited)   staticInit();

	OmnTagFuncInfo << endl;
	AosTaskDataType::E type = AosTaskDataType::toEnum(def->getAttrStr(AOSTAG_TYPE));
	if (!AosTaskDataType::isValid(type))
	{
		AosSetErrorU(rdata, "invalid_task_id") << ": " << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskDataObjPtr data = sgTaskData[type];
	if (!data)
	{
		AosSetErrorU(rdata, "task_not_defined") << ": " << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return data->create(def, rdata);
}

bool
AosTaskData::serializeTo(
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	if(!sgInited) staticInit();
	buff->setOmnStr(AosTaskDataType::toStr(mType));
	return true;
}

bool
AosTaskData::serializeFrom(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosTaskData::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);
	if(!sgInited) staticInit();

	xml->setAttr(AOSTAG_TYPE, AosTaskDataType::toStr(mType));
	return true;	
}


bool
AosTaskData::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	//taskdocid is 0, means the taskdata is in maptask
	aos_assert_r(xml, false);
	return true;	
}

AosTaskDataObjPtr
AosTaskData::serializeFromBuffStatic(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, 0);
	if(!sgInited) staticInit();

	OmnTagFuncInfo << endl;
	AosTaskDataType::E type = AosTaskDataType::toEnum(buff->getOmnStr(""));
	if (!AosTaskDataType::isValid(type))
	{
		OmnAlarm << "Invalid task Data id: " << type << enderr;
		return 0;
	}

	AosTaskDataObjPtr data = sgTaskData[type];
	if (!data)
	{
		AosSetErrorU(rdata, "task_not_defined") << ": " << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskDataObjPtr dd = data->create();
	aos_assert_r(dd, 0);

	bool rslt = dd->serializeFrom(buff, rdata);
	aos_assert_r(rslt, 0);

	return dd;

}

	
AosTaskDataObjPtr
AosTaskData::serializeFromStatic(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, 0);

	if(!sgInited) staticInit();

	AosTaskDataType::E type = AosTaskDataType::toEnum(xml->getAttrStr(AOSTAG_TYPE));
	if (!AosTaskDataType::isValid(type))
	{
		OmnAlarm << "Invalid task Data id: " << type << enderr;
		return 0;
	}

	AosTaskDataObjPtr data = sgTaskData[type];
	if (!data)
	{
		AosSetErrorU(rdata, "task_not_defined") << ": " << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskDataObjPtr dd = data->create();
	aos_assert_r(dd, 0);

	bool rslt = dd->serializeFrom(xml, rdata);
	aos_assert_r(rslt, 0);

	return dd;
}

bool
AosTaskData::isDataExist(const AosRundataPtr &rdata)
{
	return true;
}
	

AosDataCacherObjPtr
AosTaskData::createDataCacher(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

AosDataScannerObjPtr 
AosTaskData::createDataScanner(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

bool
AosTaskData::isTheSameTaskData(const AosTaskDataObjPtr &task_data, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


OmnString 
AosTaskData::getStrKey(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}
