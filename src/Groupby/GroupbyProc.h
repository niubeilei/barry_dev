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
// 05/23/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Groupby_GroupbyProc_h
#define Aos_Groupby_GroupbyProc_h

#include "Conds/Ptrs.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Groupby/GroupbyOpr.h"
#include "Groupby/GroupField.h"
#include "Groupby/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosGroupbyProc : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr					mLock;
	vector<AosGroupFieldPtr>	mFields;
	AosGroupbyOpr::E			mRecordPicker;
	int							mGroupStartPos;
	int							mGroupIdLen;
	AosFilterPtr				mFilter;

	OmnString				mCrtGroupId;
	OmnString				mPreGroupId;
	OmnString				mCrtRecord;
	bool					mFirstRecordCopied;
	int						mNumGroupsProcessed;
	bool					mFirstRcdInGroup;

public:
	AosGroupbyProc(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosGroupbyProc();

	AosDataProcStatus::E procRecord(
					char *record, 
					const int len, 
					const AosRundataPtr &rdata);

	AosDataProcStatus::E procRecord(
					const AosDataRecordObjPtr &record, 
					const AosRundataPtr &rdata);
	
	bool procFinished(
					const AosDataRecordObjPtr &record,
					const AosRundataPtr &rdata);
	
	static AosGroupbyProcPtr createGroupbyProc(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool isSameGroup(const char *record, const int len);
	bool modifyRecord(const char *record, const int len, const AosRundataPtr &rdata);
	bool finishAndStartGroupLocked(const char *record);
};
#endif

