////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_CreateDocThrd_h
#define Aos_SEClient_Testers_CreateDocThrd_h

#include "SearchEngine/Ptrs.h"
#include "SEClient/Ptrs.h"
#include "SEClient/Testers/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"



class AosCreateDocThrd : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftWeight = 100,
		eMinWaitMsec = 100,
		eDepth = 5
	};

private:
	AosSearchEngTesterPtr	mTorturer;
	int						mTotal;
	int						mWeight;
	int						mNumDocs;
	bool					mIsCreating;
	OmnThreadPtr			mThread;
	AosSEClientPtr			mConn;
	OmnString				mUser;
	OmnString				mCtnr;
	OmnString				mAppname;

public:
	AosCreateDocThrd(
			const AosSearchEngTesterPtr &torturer,
			const int total, 
			const int weight, 
			const OmnIpAddr &addr, 
			const int port);
	~AosCreateDocThrd();

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state,
						const OmnThreadPtr &thread);
  	virtual bool	stop();
  	virtual bool	signal(const int threadLogicId);
   	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	isCreating() const {return mIsCreating;}

private:
	bool 		createDoc(const int depth);
};
#endif

