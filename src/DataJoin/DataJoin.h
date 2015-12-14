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
#ifndef Aos_DataJoin_DataJoin_h
#define Aos_DataJoin_DataJoin_h 

#include "DataAssembler/Ptrs.h"
#include "DataJoin/Ptrs.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/IILScannerListener.h"
#include "SEInterfaces/Ptrs.h"
#include "Groupby/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"

#include <vector>
using namespace std;

class AosDataJoin : virtual public AosIILScannerListener,
					public OmnThrdShellProc 
{
	OmnDefineRCObject;

protected:
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
	
	OmnMutexPtr					mLock;
	OmnSemPtr					mSem;

	u32							mStartTime;
	u32							mEndTime;
	int							mProgress;
	bool						mFinished;
	bool						mSuccess;
	Status						mStatus;
	bool						mDataRetrieveSuccess;

	AosRundataPtr 				mRundata;
	AosDataJoinCtlrPtr			mCtlr;
	
public:
	AosDataJoin();
	AosDataJoin(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoin();

	bool			isSuccess() const {return mSuccess;}
	bool			isFinished() const {return mFinished;}
	OmnString		toString();

	// IILScannerListener Interfaces
	virtual bool dataRetrieved(
					const AosIILScannerObjPtr &scanner, 
					const AosRundataPtr &rdata);
	virtual bool dataRetrieveFailed(
					const AosIILScannerObjPtr &scanner, 
					const AosRundataPtr &rdata);
	virtual bool noMoreData(
					const AosIILScannerObjPtr &scanner, 
					const AosRundataPtr &rdata);

	// OmnThreadedShellProc Interface
	virtual bool 	procFinished();

	virtual int		getProgress() = 0;
	virtual bool	setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata) = 0;

	virtual AosDataAssemblerObjPtr	createIILAsm(
						const AosXmlTagPtr &iilasm,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);

	static AosDataJoinPtr createDataJoin(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	static bool		checkConfigStatic(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	static bool		staticInit();

private:
	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata) = 0;
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata) = 0;
};
#endif

