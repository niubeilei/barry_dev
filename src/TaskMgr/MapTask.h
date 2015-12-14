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
#ifndef AOS_TaskMgr_MapTask_h
#define AOS_TaskMgr_MapTask_h

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


class AosMapTask : public AosTask
{
	OmnString			mTaskSdocObjid;
	AosXmlTagPtr		mDataColTags;
	AosXmlTagPtr		mConfig;
	vector<ActionInfo>	mActionsInfo;
	int					mCrtActSeqno;
	
	//AosDatasetObjPtr	mDataset;	// Ketty 2013/12/25
	map<OmnString, AosDatasetObjPtr>	mDatasets;	// Ketty 2013/12/25
	vector<OmnString>					mDatasetsFromUpstream;	// Phil 2014/09/07
	
public:
	AosMapTask(const bool flag);
	~AosMapTask();

	AosXmlTagPtr	getTaskDoc(const AosRundataPtr &rdata);

	AosXmlTagPtr 	getDataColTag(
						const OmnString &data_col_id, 
						const AosRundataPtr &rdata);
	bool			setDataColTags(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

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
	virtual bool	runAction(const AosRundataPtr &rdata, void *data = 0);
	virtual void 	onThreadExit(const AosRundataPtr &rdata, void *data);
	
	virtual bool    allActionIsFinished(const AosRundataPtr &rdata);
	virtual bool	finishedAction(const AosRundataPtr &rdata);
	
	// Ketty 2013/12/23
	virtual bool getDataset(map<OmnString, AosDatasetObjPtr> &set_map){ set_map = mDatasets;  return true;};
	virtual map<OmnString, AosDatasetObjPtr> getDatasets(){ return mDatasets;}
	virtual AosXmlTagPtr getDataColTags(){ return mDataColTags; };
	virtual bool	createSnapShot(const AosRundataPtr &rdata);
	virtual bool 	startTask(const AosRundataPtr &rdata);

	virtual int getResource(const OmnString &res_name, const AosRundataPtr &rdata);

private:
	bool			initActions(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool			startAction(const AosRundataPtr &rdata);

	// Ketty 2013/12/23
	bool 	configByTaskDocVer1(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &task_doc);
	
	bool 	configDataset(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &task_doc);

};

#endif
