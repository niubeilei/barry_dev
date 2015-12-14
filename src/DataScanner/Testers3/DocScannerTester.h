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
#ifndef Aos_DataScanner_Testers_DocScannerTester_h
#define Aos_DataScanner_Testers_DocScannerTester_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "DataScanner/DataScanner.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include <vector>
using namespace std;


class AosDocScannerTester : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	map<u64, int>		mMap;
	vector<u64>			mStartDocid;
	vector<u64>			mNumDocs;
	OmnString 			mScannerId;
	OmnMutexPtr         mLock;
	OmnCondVarPtr       mCondVar;
	OmnThreadPtr        mThread;
	bool				mRun;
	int					mNum;
	u64					mTotalDocs;
	OmnString 			mObjid;
	bool				mIsRetrieve;
	AosDataCacherObjPtr	mDataCacher;
	u64 				mTotal;
	bool				mFlag;
	OmnString 			mBatchType;
public:
	AosDocScannerTester(
			const OmnString &scanner_id, 
			const OmnString &objid, 
			const bool &bb,
			const AosRundataPtr &rdata);

	~AosDocScannerTester();

	void startScanner(bool b);
private:
	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
	bool			basicTest();

	bool			docShuffle(const OmnString &scanner_id, const AosRundataPtr &rdata);

	bool			initByOnePhy(const AosRundataPtr &rdata);

	bool			initByTwoPhy(const AosRundataPtr &rdata);

	bool			initcheck(const AosRundataPtr &rdata);

	bool			createBitmapByOnePhy(
					vector<AosBitmapObjPtr> &bitmaps,
					const AosRundataPtr &rdata);

	bool			createBitmapByTwoPhy(
					vector<AosBitmapObjPtr> &bitmaps,
					const AosRundataPtr &rdata);

	bool			sendStart(
					const OmnString &scanner_id,
					const AosRundataPtr &rdata);

	bool			sendData(
					const OmnString &scanner_id,
					const vector<AosBitmapObjPtr> &bitmaps,
					const AosRundataPtr &rdata);

	bool			sendFinished(
					const OmnString &scanner_id,
					const int total_num_data_msgs,
					const AosRundataPtr &rdata);

	bool			retrieveData(
					const OmnString &scanner_id,
					const AosRundataPtr &rdata);

	bool			getDocs(
					const OmnString &scanner_id,
					const AosRundataPtr &rdata);

	AosBuffPtr		read(
						const OmnString &scanner_id,
						const u64 &distid,
						const AosRundataPtr &rdata);

	bool			parseDoc(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata);
	bool			check(const AosRundataPtr &rdata);

	bool			getDocs(const AosRundataPtr &rdata);

	bool			nextValue(const AosDataCacherObjPtr &cacher, const AosRundataPtr &rdata);
	
	bool			procData(char *data, const u32 &len, const u64 &docid, const AosRundataPtr &rdata);

	bool			config(const AosRundataPtr &rdata);
	
	bool			deleteDocs(const OmnString &scanner_id, const AosRundataPtr &rdata);
};
#endif
