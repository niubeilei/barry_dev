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
// 2012/10/16 Created by Ken
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSplitterDocIds_h
#define AOS_Job_JobSplitterDocIds_h


#include "Job/JobSplitter.h"
#include "TransClient/Ptrs.h"
#include <vector>


class AosJobSplitterDocIds : public AosJobSplitter
{
	vector<vector<u64> >		mDocids;
	OmnString					mRowDelimiter;
	AosXmlTagPtr				mQueryXml;
	bool						mInitedQuery;

public:
	AosJobSplitterDocIds(const bool flag);
	~AosJobSplitterDocIds();

	virtual bool splitTasks(
					const AosXmlTagPtr &def,
					const AosTaskDataObjPtr &task_data,
					vector<AosTaskDataObjPtr> &task_datas,
					const AosRundataPtr &rdata);
	
	virtual AosJobSplitterObjPtr create(
					const AosXmlTagPtr &sdoc,
					map<OmnString, OmnString> &job_env,
					const AosRundataPtr &rdata) const;
	
private:
	bool	config(
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);
	bool	query(const AosRundataPtr &rdata);
};
#endif

#endif
