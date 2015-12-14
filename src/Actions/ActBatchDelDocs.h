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
// 05/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActBatchDelDocs_h
#define Aos_SdocAction_ActBatchDelDocs_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosActBatchDelDocs : virtual public AosSdocAction,
						public OmnThreadedObj
{
private:
	enum Event
	{
		eInvalid,

		eFinished,
		eNoEvent,

		eMax
	};

	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;
	Event			mEvent;
	OmnString		mIILName;
	AosRundataPtr	mRundata;
	u64				mSizeId;
	bool			mFlag;

public:
	AosActBatchDelDocs(const bool flag);
	AosActBatchDelDocs(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosActBatchDelDocs();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	bool deleteDocs(const bool is_sync, const AosRundataPtr &rdata);
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

