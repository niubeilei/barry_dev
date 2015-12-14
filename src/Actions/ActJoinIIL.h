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
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActJoinIIL_h
#define Aos_SdocAction_ActJoinIIL_h
/*
#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/Ptrs.h"
#include "IILAssembler/Ptrs.h"
#include "IILScanner/IILMatchType.h"
#include "IILScanner/Ptrs.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/TaskObj.h"
#include "SEUtil/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include <vector>
using namespace std;

class AosActJoinIIL : virtual public AosSdocAction,
					  public AosIILScannerListener,
					  public OmnThreadedObj
{
private:
	enum
	{
		eKeyMaxLen = 10000
	};

	enum Status
	{
		eInvalid,

		eIdle,
		eWait,
		eActive,
		eFinished,
	};

	struct WorkingData
	{
		const char *key;
		int			len;
		int			idx;
		bool		is_smallest;
	};

	u32							mStartTime;
	u32							mEndTime;

	Status						mStatus;
	vector<AosIILScannerObjPtr>	mIILScanners;
	// vector<AosIILMatchType::E>	mMatchTypes;
	vector<WorkingData>			mWorkingData;
	vector<WorkingData>			mNewData;
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr 				mThread;
	AosRundataPtr 				mRundata;
	int							mNumScanners;
	bool						mDataRetrieveSuccess;
	AosTableAssemblerPtr		mTableAssembler;
	AosDataRecordObjPtr			mRecord;
	AosGroupbyProcPtr			mGroupbyProc;
	AosDataAssemblerPtr			mIILAssembler;
	AosDataAssemblerPtr			mDocAssembler;
	int							mPrefixLen;		// Chen Ding, 06/11/2012

public:
	AosActJoinIIL(const bool flag);
	AosActJoinIIL(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosActJoinIIL();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool run(const AosTaskObjPtr &task, 
					 const AosXmlTagPtr &sdoc,
					 const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// AosIILScannerListener Interface
	virtual bool dataRetrieved(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata);
	virtual bool dataRetrieveFailed(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata);
	virtual bool noMoreData(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata);

private:
	bool mergeData(const int num_values);
	bool setSmallestFlag(const u32 nn);
	bool addToWorkingData(
					vector<AosActJoinIIL::WorkingData> &working_data, 
					int &num_values,
					const char *data, 
					const int len,
					const u32 idx);
	bool retrieveData(const int idx, const AosRundataPtr &rdata);
	OmnString toString() const;
	bool allDataReceived() const;
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	
	bool actionFinished();
	bool actionFailed();
	bool sendAll();
	void printWorkingData(const int num_new_data);
};
*/
#endif

