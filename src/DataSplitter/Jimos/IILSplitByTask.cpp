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
// 2015/08/21 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/IILSplitByTask.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Job/JobMgr.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIILSplitByTask_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIILSplitByTask(version);
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


AosIILSplitByTask::AosIILSplitByTask(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_IILBYTASK, version)
{
}


AosIILSplitByTask::AosIILSplitByTask(
		const OmnString &type, 
		const u32 version)
:
AosDataSplitter(type, version)
{
}

bool
AosIILSplitByTask::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc)
{
	// mConfig is dataconnector's conf
	mIILName = worker_doc->getAttrStr(AOSTAG_IILNAME, "");
	mConfig = worker_doc->clone(AosMemoryCheckerArgsBegin);

	AosXmlTagPtr split_tag = mConfig->getFirstChild();
	aos_assert_r(split_tag, false);
	mEntries = split_tag->getAttrU64("zky_entries", eEntries);
	mConfig->removeNode(split_tag);
	return true;
}


bool
AosIILSplitByTask::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosIILSplitByTask::~AosIILSplitByTask()
{
}


bool 
AosIILSplitByTask::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	// each data_units conf format
	//<dataconnector zky_iilname="" zky_with_value="false">
	//<query_cond zky_opr="an">
	//<zky_value><![CDATA[*]]></zky_value>
	//</query_cond>
	//</dataconnector>
	
	if (mIILName == "")
		return true;
	AosXmlTagPtr job_doc = AosGetDocByDocid(mJobDocid, rdata);
	aos_assert_r(job_doc, false);
	AosXmlTagPtr scheduler_tag = job_doc->getFirstChild("scheduler");
	aos_assert_r(scheduler_tag, false);
	u32 num_slots = scheduler_tag->getAttrU32("zky_numslots", 0);
	u32 num = num_slots * AosGetNumPhysicals();
	if (num == 1)
	{
		AosXmlTagPtr config = mConfig->clone(AosMemoryCheckerArgsBegin);
		data_units.push_back(mConfig);
		return true;
	}
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(query_context, false);
	AosXmlTagPtr query_cond = mConfig->getFirstChild("query_cond");
	aos_assert_r(query_cond, false);
	OmnString opr_str = query_cond->getAttrStr("zky_opr");
	AosOpr opr = AosOpr_toEnum(opr_str);
	query_context->setOpr(opr);
	AosXmlTagPtr value1_tag = query_cond->getFirstChild("zky_value");
	if (value1_tag)
	{
		OmnString value1 = value1_tag->getNodeText();
		query_context->setStrValue(value1);
	}
	AosXmlTagPtr value2_tag = query_cond->getFirstChild("zky_value2");
	if (value2_tag)
	{
		OmnString value2 = value2_tag->getNodeText();
		query_context->setStrValue2(value2);
	}
	query_context->setSplitValueType(AosQueryContextObj::eKey);
	query_context->setBlockSize(mEntries);
OmnScreen << "all svr task total: " << num << endl;
	vector<AosQueryContextObjPtr> contexts;
	bool rslt = AosGetSplitValue(mIILName, query_context, num, contexts, rdata);
	u32 size = contexts.size();
OmnScreen << "split context size: " << size << endl;
	if (!rslt)
	{
		contexts.clear();
		contexts.push_back(query_context);
	}
	if (size == 1)
	{
		AosXmlTagPtr config = mConfig->clone(AosMemoryCheckerArgsBegin);
		data_units.push_back(mConfig);
		return true;
	}
	aos_assert_r(size > 1, false);
	for (u32 i=0; i<size; i++)
	{
		AosXmlTagPtr config = mConfig->clone(AosMemoryCheckerArgsBegin);

		AosXmlTagPtr query_cond = config->getFirstChild("query_cond");
		aos_assert_r(query_cond, false);

		AosOpr opr = contexts[i]->getOpr();
		OmnString value1, value2;
		if (i == 0)
		{
			aos_assert_r(opr == eAosOpr_lt, false);
			value1 = contexts[i]->getStrValue();
		}
		else if (i == size - 1)
		{
			aos_assert_r(opr == eAosOpr_ge, false);
			value1 = contexts[i]->getStrValue();
		}
		else
		{
			aos_assert_r(opr == eAosOpr_range_ge_lt, false);
			value1 = contexts[i]->getStrValue();
			AosOpr pre_opr = contexts[i-1]->getOpr();
			if (AosOpr_isRangeOpr(pre_opr))
			{
				aos_assert_r(value1 == contexts[i-1]->getStrValue2(), false);
			}
			else
			{
				aos_assert_r(value1 == contexts[i-1]->getStrValue(), false);
			}
			value2 = contexts[i]->getStrValue2();
		}
		query_cond->setAttr("zky_opr", AosOpr_toStr(opr));
		query_cond->setNodeText("zky_value", value1, true);
		if (value2 != "")
		{
			query_cond->setNodeText("zky_value2", value2, true);
		}
OmnScreen << "split iil query cond: " << config->toString() << endl;
		data_units.push_back(config);
	}
	return true;
}


AosJimoPtr 
AosIILSplitByTask::cloneJimo() const
{
	try
	{
		return OmnNew AosIILSplitByTask(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
