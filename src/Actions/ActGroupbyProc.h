////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_ActGroupbyProc_h
#define Aos_Actions_ActGroupbyProc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Conds/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "DataField/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "RemoteBackupClt/Ptrs.h"
#include "Groupby/GroupbyOpr.h"
#include "Groupby/GroupField.h"
#include "Groupby/Ptrs.h"
#include <vector>
using namespace std;

class AosActGroupbyProc : virtual public AosSdocAction
{
private:
	OmnMutexPtr				mLock;
	bool					mGroupIdLenFixed;
	int						mGroupIdLen;
	int						mGroupStartPos;
	AosGroupbyOpr::E    	mUncondRecordPicker;
	AosGroupbyOpr::E    	mCondRecordPicker;
	AosFilterPtr        	mFilter;
	vector<AosGroupFieldPtr>  mFields;
	AosDataRecordObjPtr		mRecord;
	char *					mCrtRecordChar;
	char *					mPreGroupRecordChar;
	char *					mCrtGroupId;
	int						mRecordLen;
	int						mCrtGroupidLen;
	int						mPreGroupidLen;
	char *					mPreGroupId;
	int						mNumRecordsProcessed;
	AosDataBlobObjPtr		mTable;

public:
	AosActGroupbyProc(const bool flag);
	AosActGroupbyProc(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActGroupbyProc();

	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	inline int getGroupIdLen(const char *group_id)
	{
		if (mGroupIdLenFixed) return mGroupIdLen;
		int len = strlen(group_id);
		if (len > mGroupIdLen) len = mGroupIdLen;
		return len;
	}

	char *getGroupId(char *record)
	{
		return &record[mGroupStartPos];
	}
	bool processLastGroup();
	bool processRecord(const AosRundataPtr &rdata);
	bool checkGroup(const AosRundataPtr &rdata);
	bool isSameGroup(const AosRundataPtr &rdata);
};
#endif

