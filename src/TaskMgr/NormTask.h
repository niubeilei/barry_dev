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
// 08/09/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_NormTask_h
#define AOS_TaskMgr_NormTask_h

#include "Actions/Ptrs.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "TaskMgr/Ptrs.h"
#include "TaskMgr/TaskThrd.h"
#include "TaskMgr/Task.h"
#include "TaskUtil/TaskStatus.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosNormTask : public AosTask
{
	AosXmlTagPtr		mDataColTags;
	AosXmlTagPtr		mConfig;
	vector<ActionInfo>	mActionsInfo;
	int					mCrtActSeqno;
	map<OmnString, AosDatasetObjPtr>	mDatasets;
	map<int, OmnString>	mIndexs;

public:
	AosNormTask(const bool flag);
	~AosNormTask();

	//AosXmlTagPtr 	getDataColTag(
	//					const OmnString &data_col_id, 
	//					const AosRundataPtr &rdata);
	//bool			setDataColTags(
	//					const AosXmlTagPtr &def,
	//					const AosRundataPtr &rdata);

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
	//TaskAction
	virtual AosTaskObjPtr create();

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
	virtual void 	onThreadExit(const AosRundataPtr &rdata, void *data);
	
	virtual bool    allActionIsFinished(const AosRundataPtr &rdata);
	virtual bool	finishedAction(const AosRundataPtr &rdata);
	
	virtual map<OmnString, AosDatasetObjPtr> getDatasets(){ return mDatasets; }
	virtual map<int, OmnString> getIndexs(){ return mIndexs; }
	virtual AosXmlTagPtr getDataColTags();

	virtual int getResource(const OmnString &res_name, const AosRundataPtr &rdata);

private:
	bool			initActions(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	bool 	configByTaskDocVer1(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &task_doc);
	
	bool 	configDataset(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &task_doc);
};

#endif
