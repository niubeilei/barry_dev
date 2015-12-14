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
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSplitter_h
#define AOS_Job_JobSplitter_h

#include "Job/Ptrs.h"
#include "SEInterfaces/JobSplitterObj.h"
#include "SEInterfaces/JobObj.h"


class AosJobSplitter : public AosJobSplitterObj
{
protected:
	OmnString					mName;
	AosJobSplitterId::E 		mType;
	map<OmnString, OmnString> 	mJobENV;

public:
	AosJobSplitter(
			const OmnString &name,
			const AosJobSplitterId::E type,
			const bool flag);
	~AosJobSplitter();

	// Chen Ding, 2013/06/02
	virtual AosJobSplitterId::E getJobSplitterId() const {return mType;}
	virtual void setJobENV(map<OmnString, OmnString> &job_env) { mJobENV= job_env;}

	// JobSplitter Interface
	virtual bool splitTasks(
					const AosXmlTagPtr &def,
					const AosTaskDataObjPtr &task_data,
					vector<AosTaskDataObjPtr> &tasks,
					const AosRundataPtr &rdata);

	virtual AosJobSplitterObjPtr create(
					const AosXmlTagPtr &sdoc,
					map<OmnString, OmnString> &job_env,
					const AosRundataPtr &) const = 0;

	virtual bool init();
	virtual void clear();

	virtual AosJobSplitterObjPtr createJobSplitter(
					const AosXmlTagPtr &sdoc, 
					map<OmnString, OmnString> &job_env,
					const AosRundataPtr &rdata);

private:
	bool	registerJobSplitter(
					AosJobSplitter *splitter,
					const OmnString &name);

	virtual bool config(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata) = 0;
};
#endif

#endif
