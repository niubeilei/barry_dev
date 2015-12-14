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
#ifndef Aos_DataJoin_DataJoinRank_h
#define Aos_DataJoin_DataJoinRank_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinRank : public AosDataJoin
{
	u64							mCrtMonth;
	OmnString					mKeyHead;
	OmnString					mMonthHead;
	OmnString					mSep;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mIILAssembler;

	multimap<int64_t, OmnString>	mValueMap;

public:
	AosDataJoinRank();
	AosDataJoinRank(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinRank();

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
	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	
	bool			getNextKey(
						u64 &month,
						OmnString &id,
						int64_t &value,
						const AosRundataPtr &rdata);
	bool			appendAllEntry(const AosRundataPtr &rdata);
	void			clear();
};

#endif

