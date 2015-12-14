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
// 07/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActJoinIILMultiThrd_h
#define Aos_SdocAction_ActJoinIILMultiThrd_h
/*
#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/Ptrs.h"
#include "IILAssembler/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/Ptrs.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/TaskObj.h"
#include "SEUtil/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include <vector>
using namespace std;

class AosActJoinIILMultiThrd : virtual public AosSdocAction,
					  public OmnThreadedObj
{
private:
	enum
	{
		eKeyMaxLen = 10000
	};



	u32							mStartTime;
	u32							mEndTime;
	vector<AosIILScannerObjPtr>	mIILScanners;
	AosRundataPtr 				mRundata;
	AosDataRecordObjPtr			mRecord;
	AosDataAssemblerPtr			mIILAssembler;
	AosDataAssemblerPtr			mDocAssembler;
	int							mPrefixLen;		// Chen Ding, 06/11/2012
	bool						mRunInMultiThreads;
	AosGroupbyProcPtr			mGroupbyProc;

	static u32 smMaxEntriesPerThread;

public:
	AosActJoinIILMultiThrd(const bool flag);
	AosActJoinIILMultiThrd(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosActJoinIILMultiThrd();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool run(const AosTaskObjPtr &task, 
					 const AosXmlTagPtr &sdoc,
					 const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool actionFinished();
	bool actionFailed();

private:
	bool retrieveData(const int idx, const AosRundataPtr &rdata);
	bool mergeData(const int num_values);
	bool setSmallestFlag(const u32 nn);
	OmnString toString() const;
	bool allDataReceived() const;
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	
	bool sendAll();
	void printWorkingData(const int num_new_data);
	bool runInSingleThread(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool runInMultiThreads(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	bool createLastThreadEntry(
					const OmnString &start_key,
					const u64 &start_docid,
					const u32 controlling_iilidx, 
					vector<OmnThrdShellProcPtr> &runners,
					const AosRundataPtr &rdata);

	bool createOneThreadEntry(
					const OmnString &start_key,
					const u64 &start_docid,
					const OmnString &end_key,
					const u64 &end_docid,
					const u32 controlling_iilidx, 
					vector<OmnThrdShellProcPtr> &runners,
					const AosRundataPtr &rdata);
};
*/
#endif

