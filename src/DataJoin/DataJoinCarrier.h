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
// 2014/01/18 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_DataJoinCarrier_h
#define Aos_DataJoin_DataJoinCarrier_h 

#include "DataJoin/DataJoin.h"


class AosDataJoinCarrier : public AosDataJoin
{
	OmnString					mKeyHead;
	OmnString					mSep;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mIILAsm;

	u64							mCrtTime;
	u64							mCrtNum;

public:
	AosDataJoinCarrier();
	AosDataJoinCarrier(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinCarrier();

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
						u64 &time,
						const AosRundataPtr &rdata);
	void			clear();
};

#endif

