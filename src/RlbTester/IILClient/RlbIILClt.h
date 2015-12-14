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
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_IILClient_RlbIILClt_h
#define Aos_RlbTester_IILClient_RlbIILClt_h 

#include "Rundata/Ptrs.h"
#include "RlbTester/Util/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/LRUCache.h"
#include "Util/Ptrs.h"

#include <map>
using namespace std;


class AosRlbIILClt: public OmnThreadedObj
{
	OmnDefineRCObject;
	
	enum
	{
		eMonitorThrdId,
	
		eAddWeight = 40, 
		eModifyWeight = 20,
		eCheckWeight = 20,
	};
	
	typedef map<u32, AosTesterCubeGrpInfoPtr>				map_t;
	typedef map<u32, AosTesterCubeGrpInfoPtr>::iterator 	mapitr_t;
private:
	OmnMutexPtr	 	mLock;
	u64				mDocid;
	int 			mNumDocs;
	
	map_t			mCubeGrpInfo;
	OmnThreadPtr	mMonitorThrd;
	bool			mShowLog;

public:
	AosRlbIILClt();
	~AosRlbIILClt();

	bool		start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	bool 	basicTest(const AosRundataPtr &rdata);
	bool 	monitorThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	bool 	addEntry(const AosRundataPtr &rdata);
	bool 	removeEntry(const AosRundataPtr &rdata);
	bool 	check(const AosRundataPtr &rdata);

	AosTesterCubeGrpInfoPtr randGetCubeGrpInfo();

	OmnString createIILName();

};
#endif
