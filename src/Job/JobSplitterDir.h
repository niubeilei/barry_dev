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
#ifndef AOS_Job_JobSplitterDir_h
#define AOS_Job_JobSplitterDir_h


#include "Job/JobSplitter.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "Util/CodeConvertion.h"

#include <map>
#include <queue>
#include <vector>


class AosJobSplitterDir : public AosJobSplitter
{
	enum 
	{
		eMaxDirNum = 200,
		
		eDftSplitFileNum = 1000,
		eMinSplitFileNum = 500,
		eMaxSplitFileNum = 10000,

		eDftSplitSize = 20000000000LL,	// 20G
		eMinSplitSize = 500000000LL,	// 500M
		eMaxSplitSize = 200000000000LL	// 200G
	};

	bool			mSplitByDir;
	int64_t			mSplitSize;
	int64_t			mSplitFileNum;				
	bool			mRecursion;
	bool			mIgnoreHead;
	OmnString 		mCharacterType;	
	OmnString		mRowDelimiter;

	bool			mIsUnicomFile;
	OmnString		mExtension;

public:
	AosJobSplitterDir(const bool flag);
	~AosJobSplitterDir();

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

	bool	split(
				vector<OmnString> &dir_paths,
				const int physical_id,
				const bool recursion,
				const bool ignore_head,
				const OmnString &character_type,
				const OmnString &row_delimiter,
				vector<AosTaskDataObjPtr> &task_datas,
				const AosRundataPtr &rdata);

	bool	splitNormal(
				vector<AosTaskDataObjPtr> &task_datas,
				map<int64_t, queue<AosFileInfo> > &dev_map,
				const int physical_id,
				const bool ignore_head,
				const OmnString &character_type,
				const OmnString &row_delimiter,
				const AosRundataPtr &rdata);

	bool 	splitUnicomFileByNum(
				vector<AosTaskDataObjPtr> &task_datas,
				map<int64_t, queue<AosFileInfo> > &dev_map,
				const int physical_id,
				const bool ignore_head,
				const OmnString &character_type,
				const OmnString &row_delimiter,
				const AosRundataPtr &rdata);

};
#endif

#endif
