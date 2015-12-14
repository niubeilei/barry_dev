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
#ifndef Aos_DataJoin_DataJoinCellFluctuate_h
#define Aos_DataJoin_DataJoinCellFluctuate_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinCellFluctuate : public AosDataJoin
{
	u64							mTimeFrame;
	OmnString					mCrtKey;
	u64							mFirstDay;
	OmnString					mKeyHead;
	OmnString					mKeyHead2;
	OmnString					mSep;

	AosIILEntryMapPtr			mKeyCityMap;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mIILAsm;
	AosDataAssemblerObjPtr		mIILAsm2;

	vector<u64>					mValueVector;

public:
	AosDataJoinCellFluctuate();
	AosDataJoinCellFluctuate(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinCellFluctuate();

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
	
	bool			getNextKey(
						OmnString &key,
						u64 &month,
						u64 &value,
						bool &finish,
						const AosRundataPtr &rdata);
	bool			appendRankEntry(
						const u64 &month,
						const OmnString &id,
						const u64 &value,
						const AosRundataPtr &rdata);
	bool			appendAllEntry(AosRundata *rdata);
	void			clear();
};

#endif

