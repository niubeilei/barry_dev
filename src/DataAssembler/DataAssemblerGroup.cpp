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
// 06/13/2012 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/DataAssemblerGroup.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "DataAssembler/DataAssemblerSort.h"
#include "TaskMgr/Task.h"


AosDataAssemblerGroup::AosDataAssemblerGroup(
		const OmnString &asm_key,
		const u64 task_docid)
:
AosDataAssembler(AosDataAssemblerType::eGroup, AOSDATAASSEMBLER_GROUP, asm_key, task_docid),
mShuffleNum(-1),
mShuffleId(-1),
mRawShuffleField(0),
mLock(OmnNew OmnMutex()),
mRawLock(mLock.getPtr())
{
}


AosDataAssemblerGroup::~AosDataAssemblerGroup()
{
}


bool
AosDataAssemblerGroup::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	bool rslt = AosDataAssembler::config(def, rdata);
	aos_assert_r(rslt, false);

	mShuffleType = def->getAttrStr(AOSTAG_SHUFFLE_TYPE);
	aos_assert_r(mShuffleType != "", false);
	int num = -1;
	if (mShuffleType == "cube")
	{
		num = AosGetNumCubes();
	}
	else if (mShuffleType == "physical")
	{
		num = AosGetNumPhysicals();
	}
	else
	{
		num = mShuffleType.toInt();
	}
	mShuffleNum = num;
	OmnString expr_str = def->getAttrStr(AOSTAG_SHUFFLE_VALUE);
	if (expr_str != "")
	{
		OmnString error;
		expr_str << ";";
		mShuffleField = AosParseExpr(expr_str, error, rdata);
		if (!mShuffleField)
		{
			OmnAlarm << error << enderr;
			return -1;
		}
		mRawShuffleField = mShuffleField.getPtr();
	}

	AosDataAssemblerObjPtr assembler;
	OmnString asm_key;
	for (int i = 0; i < num; i++)
	{
		asm_key = "";
		asm_key << mAsmKey << "_" << i;
		assembler = AosDataAssemblerSort::createAssemblerSort(asm_key, mTaskDocid, def, rdata);
		mAssemblers.push_back(assembler);
		mRawAssemblers.push_back(assembler.getPtr());
	}
	return true;
}


bool
AosDataAssemblerGroup::sendStart(AosRundata *rdata)
{
	for (u32 i = 0; i < mRawAssemblers.size(); i++)
	{
		mRawAssemblers[i]->setTargetReporter(this->mTargetReporter);
		mRawAssemblers[i]->sendStart(rdata);
	}

	return true;
}

	
bool
AosDataAssemblerGroup::sendFinish(AosRundata *rdata)
{
	for (u32 i = 0; i < mRawAssemblers.size(); i++)
	{
		mRawAssemblers[i]->sendFinish(rdata);
	}
	return true;	
}

bool
AosDataAssemblerGroup::waitFinish(AosRundata *rdata)
{
	for (u32 i = 0; i < mRawAssemblers.size(); i++)
	{
		mRawAssemblers[i]->waitFinish(rdata);
	}
	return true;	
}


bool
AosDataAssemblerGroup::appendEntry(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	int shuffle_id = getShuffleId(record, rdata);
	mRawAssemblers[shuffle_id]->appendEntry(rdata, record);
	return true;
}


AosDataAssemblerObjPtr
AosDataAssemblerGroup::createAssembler(
		const OmnString &asm_key, 
		const u64 task_docid, 
		const AosXmlTagPtr &def, 
		AosRundata* rdata)
{
	AosDataAssemblerGroup* asm_sort = OmnNew AosDataAssemblerGroup(asm_key, task_docid);
	bool rslt = asm_sort->config(def, rdata);
	aos_assert_r(rslt, 0);
	return asm_sort;
}

bool
AosDataAssemblerGroup::checkConfigStatic(
		const AosXmlTagPtr &def,
		AosRundata* rdata)
{
	return true;
}

int
AosDataAssemblerGroup::getShuffleId(
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	if (!mRawShuffleField)
	{
		if (mShuffleId > -1)
			return mShuffleId;
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		OmnString value = task->getTaskENV(AOSTAG_SHUFFLE_ID, rdata);
		mShuffleId = value.toInt();
		aos_assert_r(mShuffleId >= 0, -1);
		return mShuffleId;
	}
	mRawLock->lock();
	bool rslt = mRawShuffleField->getValue(rdata, record, mValue);
	aos_assert_rl(rslt, mRawLock, -1);
	i64  vv = mValue.getI64();
	mRawLock->unlock();
	return vv;
}

