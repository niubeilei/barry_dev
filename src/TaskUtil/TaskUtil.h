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
#ifndef AOS_TaskUtil_TaskUtil_h
#define AOS_TaskUtil_TaskUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "TaskUtil/TaskStatus.h"
#include "SEInterfaces/TaskDataObj.h"

#include <map>

class AosTaskUtil
{
public:
	static bool  modifyFailedTaskDoc(
					const AosXmlTagPtr &task_doc,
					const OmnString &str_type,
					const OmnString &error_msg,
					const int svr_id,
					const AosRundataPtr &rdata);

	static bool	setStatus(
					const AosXmlTagPtr &task_doc,
					const AosTaskStatus::E &status,
					const AosRundataPtr &rdata);

	static int	nextPhysicalsId(const int phy);

	static bool	stopTaskProcess(
					const int serverid, 
					const u64 &task_docid, 
					const AosRundataPtr &rdata);

	static AosXmlTagPtr getErrorEntry(
					const AosXmlTagPtr &task_doc,
					int &error_num,
					const AosRundataPtr &rdata);

	static AosXmlTagPtr	 serializeSnapShotsTo(
					map<u64, map<OmnString, AosTaskDataObjPtr> > &snapshots, 
					const AosRundataPtr &rdata);

	static bool serializeSnapShotsFrom(
					map<OmnString, AosTaskDataObjPtr> &snapshots, 
					const AosXmlTagPtr &snapshot_tag,
					const AosRundataPtr &rdata);

};
#endif
