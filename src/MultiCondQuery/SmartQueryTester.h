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
// 	Created: 2015/03/11	by liuwei
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClient_Tester2_IILI64Tester_h
#define AOS_IILClient_Tester2_IILI64Tester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "BitmapMgr/Bitmap.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "IILUtil/IILTypes.h"
#include "MultiCondQuery/MCField.h"
#include "MultiCondQuery/Ptrs.h"
#include "NativeAPINew/zykie.h"

#include "list"

class AosSmartQueryTester : public OmnTestPkg
{
private:
	
	enum
	{
		eMinGroupSize = 1,
		eMaxGroupSize = 1000000,
		eMaxBlockSizes = 1000000,
	};
	AosRundataPtr 	mRundata;
	AosQueryRsltObjPtr mQuery_rslt;
	AosOpr			mOpr;

	vector<AosMCFieldPtr>	 mMCFields;
	u64				mNumRecords;
	u64				mNumFields;
	u64				mNumber;
	u64				mTime;
	OmnString		mName;
	bool			mDirty;
	u32				mSeed;
	int 			mStep;
	OmnString 		mZeroStr;
	bool*			mQueryFlag;
	AosBitmapPtr	mBitmap;			
	u64 			mMaxTid;
	OmnString		mTableName;
	OmnMutexPtr             mLock;
	OmnCondVarPtr           mCondVar;

	ZYKIE*			mConn;
	ZYKIE_RES*		mResSet;
	ZYKIE_ROW		mRow;
	OmnString		mOptHostName;
	OmnString		mUserName;
	OmnString		mPassWd;
	u64				mRemotePort;
public:
	AosSmartQueryTester();
	~AosSmartQueryTester();
	virtual bool 	start();
	virtual bool	query();
	void			setStep(const int &step){mStep = step;}
	bool			resp(const AosXmlTagPtr &rslt);
private:
	bool 			config();
	bool 			createFields();
	u64				getValue();
	AosOpr 			selectOpr();
	void			changeValue(u64&,u64&);
	bool			checkTID(const u64);
	bool			checkCond(const u64);
	bool			loadData();
	u64				randMoveToStep();
	u64				randQueryCount();
	bool			percent(const u64);
	u64				randnum(const u64, const u64);
	u64 			random_block_size(const u64 &remain);
	OmnString 		toString(u64);
	u64				randCheckNum();
	u64				randPageSize();
	u64				charToU64(char *);
	bool			matchFieldConds(const u64);
	bool			inRange(const u64);
	bool	percent(const int i)
	{
		return (rand()%100 < i);
	}
};
#endif
