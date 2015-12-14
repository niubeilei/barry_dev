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
#if 0
#ifndef Aos_DataJoin_DataJoinNetStatus_h
#define Aos_DataJoin_DataJoinNetStatus_h 

#include "DataJoin/DataJoin.h"


class AosDataJoinNetStatus : public AosDataJoin
{
	bool						mProcAll;
	u64							mEpochDay;
	int							mKeyFieldIdx;
	int							mCityFieldIdx;
	int							mCarrierFieldIdx;
	int							mDayFieldIdx;
	OmnString					mFieldSep;
	OmnString					mSep;
	u16							mKeyHead;
	u16							mCityHead;

	bool						mIsFirstKey;
	bool						mNeedMove;

	OmnString					mCrtKey;
	OmnString					mCrtCity;
	OmnString					mCrtCarrierId;
	u64							mCrtDay;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mInNetIILAsm;
	AosDataAssemblerObjPtr		mOnNetIILAsm;
	AosDataAssemblerObjPtr		mOutNetIILAsm;
	AosDataAssemblerObjPtr		mInNetStIILAsm;
	AosDataAssemblerObjPtr		mOnNetStIILAsm;
	AosDataAssemblerObjPtr		mOutNetStIILAsm;
	
	AosIILScannerObjPtr			mOnNetIILScanner;

public:
	AosDataJoinNetStatus();
	AosDataJoinNetStatus(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinNetStatus();

	// OmnThreadedShellProc Interface
	virtual	bool	run();
	
	int				getProgress();
	bool			setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata);
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			runJoin(const AosRundataPtr &rdata);
	bool			sendStart(const AosRundataPtr &rdata);
	bool			sendFinish(const AosRundataPtr &rdata);
	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	
	bool			procNewKey(
						const OmnString &key,
						const AosRundataPtr &rdata);
	bool			procKeyAll(
						const OmnString &key,
						const OmnString &city,
						const OmnString &carrier_id,
						const u64 &day,
						const AosRundataPtr &rdata);
	bool			procKeyDay(
						const OmnString &key,
						const OmnString &city,
						const OmnString &carrier_id,
						const u64 &day,
						const AosRundataPtr &rdata);
	bool			procNextOnNetKey(
						const OmnString &key,
						const bool procToEnd,
						const AosRundataPtr &rdata);
	bool			procEnd(const AosRundataPtr &rdata);
	bool			appendInNetEntry(
						const OmnString &key,
						const OmnString &city,
						const OmnString &carrier_id,
						const u64 &day,
						const AosRundataPtr &rdata);
	bool			appendOnNetEntry(
						const OmnString &key,
						const OmnString &city,
						const OmnString &carrier_id,
						const u64 &day,
						const AosRundataPtr &rdata);
	bool			appendOutNetEntry(
						const OmnString &key,
						const OmnString &city,
						const OmnString &carrier_id,
						const u64 &day,
						const AosRundataPtr &rdata);
	void			clear();
};

#endif
#endif

