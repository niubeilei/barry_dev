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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskMgr_TaskDataUnicomDir_h
#define AOS_TaskMgr_TaskDataUnicomDir_h

#include "TaskMgr/TaskData.h"

#include <vector>
using namespace std;


class AosTaskDataUnicomDir : public AosTaskData
{
	int						mPhysicalId;
	bool					mRecursion;
	bool					mIgnoreHead;
	OmnString				mCharacterType;
	OmnString				mRowDelimiter;

	bool					mIsFiles;

	bool					mIsUnicomFile;

	vector<OmnString>		mDirPaths;
	vector<AosFileInfo>	mFileInfos;

public:
	AosTaskDataUnicomDir(const bool flag);
	AosTaskDataUnicomDir(
			vector<OmnString> &dir_paths,
			const int physical_id,
			const bool recursion,
			const bool ignore_head,
			const OmnString &character_type,
			const OmnString &row_delimiter);
	AosTaskDataUnicomDir(
			vector<AosFileInfo> &fileinfos,
			const int physical_id,
			const bool ignore_head,
			const OmnString &character_type,
			const OmnString &row_delimiter,
			const bool isUnicomFile);

	~AosTaskDataUnicomDir();

	virtual int			getPhysicalId() const {return mPhysicalId;}
	virtual void		setPhysicalId(const int p) {mPhysicalId = p;}

	virtual bool		serializeTo(
							AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);
	virtual bool		serializeFrom(
							const AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);
	virtual bool		isDataExist(const AosRundataPtr &rdata);

	virtual AosTaskDataObjPtr create() const;
	virtual AosTaskDataObjPtr create(
							const AosXmlTagPtr &def,
							const AosRundataPtr &rdata) const;

	virtual AosDataCacherObjPtr createDataCacher(
							const AosDataRecordObjPtr &record,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createDataScanner(const AosRundataPtr &rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif

#endif
