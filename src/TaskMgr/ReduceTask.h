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
// 2013/04/17	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_ReduceTask_h
#define AOS_TaskMgr_ReduceTask_h

#include "Actions/Ptrs.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/Ptrs.h"
#include "TaskMgr/TaskThrd.h"
#include "TaskMgr/Task.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskUtil/TaskStartType.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosReduceTask : public AosTask
{
	ActionInfo			mActionInfo;
	AosXmlTagPtr		mTaskSdoc;

public:
	AosReduceTask(const bool flag);
	~AosReduceTask();

	AosXmlTagPtr	getTaskDoc(const AosRundataPtr &rdata);

	AosXmlTagPtr 	getDataColTag(
						const OmnString &data_col_id, 
						const AosRundataPtr &rdata) {return 0;}
	bool			setDataColTags(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) {return true;}

	// AosTask pure virtual functions
	virtual bool	config(
						const AosXmlTagPtr &conf,
						const AosRundataPtr &rdata);
	virtual void	clear();
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual bool	serializeFrom(
						const AosXmlTagPtr &doc,
						const AosRundataPtr &rdata);
	virtual bool	serializeTo(
						const AosXmlTagPtr &doc,
						const AosRundataPtr &rdata);

	virtual AosTaskObjPtr create();
	virtual AosTaskObjPtr create(
						const AosXmlTagPtr &sdoc, 
						const AosJobInfo &jobinfo,
						const AosTaskDataObjPtr &task_data,
						const OmnString &logic_id,
						const AosRundataPtr &rdata);

	virtual bool	actionProgressed(
						const int percent,
						const AosRundataPtr &rdata); 
	virtual bool	actionFinished(
						const AosActionObjPtr &action,
						const AosRundataPtr &rdata);
	virtual bool	actionFailed(
						const AosActionObjPtr &action,
						const AosRundataPtr &rdata);

	virtual bool	checkTaskSuccessLock(bool &all_success);

	virtual bool 	initMemberAction(const AosRundataPtr &rdata);

	virtual void 	onThreadInit(const AosRundataPtr &rdata, void **data);
	virtual bool	runAction(const AosRundataPtr &rdata, void *data=0);
	virtual void	onThreadExit(const AosRundataPtr &rdata, void *data);

	virtual bool    allActionIsFinished(const AosRundataPtr &rdata);
	virtual bool	finishedAction(const AosRundataPtr &rdata);
	virtual bool	createSnapShot(const AosRundataPtr &rdata);

private:
	bool			initActions(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool			startAction(const AosRundataPtr &rdata);
};

#endif
