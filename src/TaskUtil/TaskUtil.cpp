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
// 08/14/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "TaskUtil/TaskUtil.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "JobTrans/StopTaskProcessTrans.h"
#include "SEUtil/DocTags.h"
#include "TaskUtil/TaskStatus.h" 
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskData.h"

bool
AosTaskUtil::modifyFailedTaskDoc(
		const AosXmlTagPtr &task_doc,
		const OmnString &str_type,
		const OmnString &error_msg,
		const int svr_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_doc, false);
	task_doc->setAttr(AOSTAG_START_STAMP, "0");
	OmnString finished_time = OmnGetTime(AosLocale::eChina);
	task_doc->setAttr(AOSTAG_END_TIME, finished_time);
	task_doc->setAttr(AOSTAG_STATUS, AOSTASKSTATUS_FAIL);
	AosXmlTagPtr error_reasons = task_doc->getFirstChild("error_reasons");
	if (!error_reasons)
	{
		error_reasons = task_doc->addNode1("error_reasons");
	}
	aos_assert_r(error_reasons, false);
	AosXmlTagPtr reason = error_reasons->addNode1("reason");
	aos_assert_r(reason, false);

	reason->setAttr(AOSTAG_ZKY_TYPE, str_type);
	reason->setAttr("zky_server_id", svr_id);
	OmnString str = "<error>";
	str << error_msg << "</error>";
	AosXmlParser parser;
	AosXmlTagPtr error = parser.parse(str, "" AosMemoryCheckerArgs);
	if (error)
	{
		reason->addNode(error);
	}
	//reason->setNodeText(error_msg, true);
	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTaskUtil::setStatus(
		const AosXmlTagPtr &task_doc,
		const AosTaskStatus::E &status,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_doc, rdata);
	task_doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(status));
	if (status == AosTaskStatus::eStart)
	{
		task_doc->setAttr(AOSTAG_START_TIME, OmnGetTime(AosLocale::eChina));
		AosModifyDoc(task_doc, rdata);
	}
	else if (status == AosTaskStatus::eFinish)
	{
		task_doc->setAttr(AOSTAG_END_TIME, OmnGetTime(AosLocale::eChina));
		task_doc->setAttr(AOSTAG_PROGRESS, "100");
		AosModifyDoc(task_doc, rdata);
	}
	return true;
}


int
AosTaskUtil::nextPhysicalsId(const int phy)
{
	if (phy+1 == AosGetNumPhysicals()) return 0;
	return phy+1;
}


bool
AosTaskUtil::stopTaskProcess(
		const int serverid, 
		const u64 &task_docid, 
		const AosRundataPtr &rdata)
{
	OmnScreen << " stop task process; server_id:" << serverid << "; task_docid:" << task_docid << ";" << endl;
	AosTransPtr trans = OmnNew AosStopTaskProcessTrans(serverid, task_docid);
	return AosSendTrans(rdata, trans);
}


AosXmlTagPtr
AosTaskUtil::getErrorEntry(
		const AosXmlTagPtr &task_doc,
		int &error_num,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_doc, 0);
	AosXmlTagPtr error_reasons = task_doc->getFirstChild("error_reasons");	
	aos_assert_r(error_reasons, 0);

	error_num = error_reasons->getNumSubtags();
	aos_assert_r(error_num > 0, 0);

	AosXmlTagPtr reason = error_reasons->getChild(error_num - 1);
	return reason;
}


AosXmlTagPtr
AosTaskUtil::serializeSnapShotsTo(
			map<u64, map<OmnString, AosTaskDataObjPtr> > &snapshots, 
			const AosRundataPtr &rdata)
{
	if (snapshots.size() == 0) return 0;

	OmnString docstr = "<job_snapshots__n/>";
	AosXmlTagPtr snapshot_tag = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	AosTaskDataObjPtr snapshot;
	map<u64, map<OmnString, AosTaskDataObjPtr> >::iterator task_itr = snapshots.begin();
	while(task_itr != snapshots.end())
	{
		map<OmnString, AosTaskDataObjPtr>::iterator iter = (task_itr->second).begin();
		while(iter != (task_itr->second).end())
		{
			AosXmlTagPtr xml;
			(iter->second)->serializeTo(xml, rdata);
			OmnString kk = iter->first;
			kk << "_" << task_itr->first;
			xml->setAttr("key", kk);
			snapshot_tag->addNode(xml);
OmnScreen << "serialize taskdocid:" << task_itr->first << ";xml:" << xml->toString() << ";" << endl;
			++ iter;
		}
		++task_itr;
	}
	return snapshot_tag;
}


bool
AosTaskUtil::serializeSnapShotsFrom(
			map<OmnString, AosTaskDataObjPtr> &snapshots, 
			const AosXmlTagPtr &snapshots_tag,
			const AosRundataPtr &rdata)
{
	if (!snapshots_tag) return true;

	AosTaskDataObjPtr snapshot = 0;
	AosXmlTagPtr snapshot_tag = snapshots_tag->getFirstChild(true);
	while(snapshot_tag)
	{
		OmnString key = snapshot_tag->getAttrStr("key", "");
		map<OmnString, AosTaskDataObjPtr>::iterator itr = snapshots.find(key);
		if (itr == snapshots.end())
		{
			snapshot = AosTaskData::serializeFromStatic(snapshot_tag, rdata);
			snapshots[key] = snapshot;
		}
		snapshot_tag = snapshots_tag->getNextChild();
	}
	return true;
}

