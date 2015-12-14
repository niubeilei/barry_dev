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
// 09/26/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_Testers_ResultAsyncTester_h
#define Aos_StorageEngine_Testers_ResultAsyncTester_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DocClientCaller.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include <vector>
using namespace std;


class AosResultAsyncTester : public AosDocClientCaller 
{
	OmnDefineRCObject;

private:

	struct AosDocids
	{
		u64 	start_docid;
		int 	num_docs;
	};

	OmnString 			mScannerId;
	vector<AosDocids>	mDocids;
	map<u64, int>		mMap;
	bool				mFinished;
	u64 				mToTalDocs;
	OmnMutexPtr 		mLock;
	OmnString			mFieldStr;
	vector<int>			mFinishedServer;

public:
	AosResultAsyncTester(
			const OmnString &scanner_id, 
			const AosRundataPtr &rdata);

	~AosResultAsyncTester();

	virtual bool docClientCallback(
			const AosRundataPtr &rdata,
			const OmnString &id,
			const AosBuffPtr &buff,
			const bool finished);

	bool			basicTest();

	bool			initDocids(const u64 &start_docid, const int &num_docs);
private:

	bool			docShuffle(const OmnString &scanner_id, const AosRundataPtr &rdata);

	bool			createBitmapByOnePhy(
					vector<AosBitmapObjPtr> &bitmaps,
					const AosRundataPtr &rdata);

	bool 	parseBuff(
				const AosRundataPtr &rdata,
				const OmnString &serverid,
				const AosBuffPtr &buff,    
				const bool finished); 
	bool 	formatText(
				const AosBuffPtr &buff,    
				const OmnString &serverid,
				const bool finished, 
				const AosRundataPtr &rdata);


	bool 	formatAll(
				const AosBuffPtr &buff,    
				const OmnString &serverid,
				const bool finished, 
				const AosRundataPtr &rdata);

	bool 	check(const bool finished, const AosRundataPtr &rdata); 

};
#endif
