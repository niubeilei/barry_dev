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
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TaskTrans_TaskTrans_h
#define Aos_TaskTrans_TaskTrans_h

#include "SEInterfaces/TaskTransObj.h"
#include <vector>
using namespace std;


class AosTaskTrans : virtual public AosTaskTransObj
{

protected:
	AosXmlTagPtr 	mTransDoc;
	bool			mIsPublic;
	OmnString		mName;
	OmnString		mTransTags;
	OmnString		mTransContainer;
	OmnString		mLogContainer;
	OmnString		mDescription;
	OmnString		mJobObjid;
	u64				mJobDocid;
	int				mJobServerId;
	int				mTransId;
	int				mNextTransId;
	int				mProgress;
	Status			mStatus;
	vector<AosTaskObjPtr>	mTasks;

public:
	AosTaskTrans(const OmnString &name, 
			const AosTaskTransType::E type,
			const bool flag);
	~AosTaskTrans();

	virtual bool addTask(const AosTaskObjPtr &task);
	virtual AosDataProcStatus::E commit(const AosRundataPtr &rdata);
};
#endif
