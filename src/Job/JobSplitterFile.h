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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSplitterFile_h
#define AOS_Job_JobSplitterFile_h

#include "Job/JobSplitter.h"


#include <vector>

class AosJobSplitterFile : public AosJobSplitter
{

public:
	AosJobSplitterFile(const bool flag);
	~AosJobSplitterFile();

	virtual bool splitTasks(
					const AosXmlTagPtr &def,
					const AosTaskDataObjPtr &task_data,
					vector<AosTaskDataObjPtr> &task_datas,
					const AosRundataPtr &rdata);
	
	virtual AosJobSplitterObjPtr create(
					const AosXmlTagPtr &sdoc,
					map<OmnString, OmnString> &job_env,
					const AosRundataPtr &) const;
	
private:
	bool	config(
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);
};
#endif

#endif
