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
// 08/21/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSplitterVirtFile_h
#define AOS_Job_JobSplitterVirtFile_h

#include "Job/JobSplitter.h"

#include "Util/File.h"
#include "TransClient/Ptrs.h"

#include <vector>

class AosJobSplitterVirtFile : public AosJobSplitter
{
	struct FileInfo
	{
		OmnString   mVirFileObjid;
		OmnString   mSdocObjid;
		OmnString   mTaskId;
	};
	
	OmnMutexPtr						mLock;
	vector<FileInfo>				mFileInfos;

public:
	AosJobSplitterVirtFile(const bool flag);
	~AosJobSplitterVirtFile();

	virtual bool splitTasks(
				map<OmnString, AosTaskDataObjPtr> &tasks,
				const AosRundataPtr &rdata);

	virtual AosJobSplitterObjPtr create(
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &) const;
	
private:
	bool	config(
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);
};
#endif

#endif
