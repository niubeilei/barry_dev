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
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSplitterAuto_h
#define AOS_Job_JobSplitterAuto_h

#include "Job/JobSplitter.h"

#include <vector>
		
class AosJobSplitterAuto : public AosJobSplitter
{
private:
	enum SplitMethod
	{
		eInvalid,

		ePerPhysical,
		ePerVirtual,
		eEnumeration,

		eMax
	};

	struct TaskInfo
	{
		OmnString	mFilename;
		OmnString	mSdocObjid;
		OmnString 	mTaskId;
		int			mPhysicalId;
	};

	AosXmlTagPtr 		mSdoc;
	OmnMutexPtr			mLock;
	SplitMethod			mMethod;
	vector<TaskInfo>	mTasksInfo;					

public:
	AosJobSplitterAuto(const bool flag);
	~AosJobSplitterAuto();

	virtual bool splitTasks(
					map<OmnString, AosTaskDataObjPtr> &tasks,
					const AosRundataPtr &rdata);
	
	virtual AosJobSplitterObjPtr create(
					const AosXmlTagPtr &sdoc,
					const AosJobObjPtr &job,
					const AosRundataPtr &) const;
	
private:
	bool	config(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	bool	configTasks(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	SplitMethod	splitMethodStr2Enum(const OmnString &str);
	bool	isValidMethod(const SplitMethod split) {return split > eInvalid && split < eMax;}
};
#endif
#if 0
