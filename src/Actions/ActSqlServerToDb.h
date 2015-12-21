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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SdocAction_ActSqlServerToDb_h
#define Aos_SdocAction_ActSqlServerToDb_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <vector>


class AosActSqlServerToDb : virtual public AosSdocAction,
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
	AosRundataPtr   mRundata;
	OmnString 		mHost;
	OmnString		mUserName;
	OmnString		mPassWord;
	OmnString 		mDbName;
	unsigned int	mPort;
	AosXmlTagPtr 	mImports;

public:
	AosActSqlServerToDb(const bool flag);
	~AosActSqlServerToDb();

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool toImport(const AosRundataPtr &rdata);
};
#endif
#endif

