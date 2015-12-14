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
#ifndef Aos_DataJoin_DataJoinOnLine_h
#define Aos_DataJoin_DataJoinOnLine_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinOnLine : public AosDataJoin
{
	u64						mValue;
	u64						mEndDay;
	OmnString				mKeyHead;
	OmnString				mKeyHead2;
	OmnString				mSep;

	AosIILEntryMapPtr		mEndDayMap;
	AosIILEntryMapPtr		mTypeMap;

	AosIILScannerObjPtr		mIILScanner;
	AosDataAssemblerObjPtr	mIILAsm;
	AosDataAssemblerObjPtr	mIILAsm2;

	map<OmnString, u64>		mDayMap;
	map<OmnString, u64>		mMonthMap;

public:
	AosDataJoinOnLine();
	AosDataJoinOnLine(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinOnLine();

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
						u64 &value,
						const AosRundataPtr &rdata);
	bool			appendOnLineEntry(
						const u64 &type,
						const u64 &start_day,
						const u64 &end_day,
						const AosRundataPtr &rdata);
	u64				getEndDay(
						const OmnString &key,
						const AosRundataPtr &rdata);
	u64				getType(
						const OmnString &key,
						const AosRundataPtr &rdata);
	void			clear();
};

#endif

