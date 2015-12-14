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
#ifndef Aos_DataJoin_DataJoinCRIDay_h
#define Aos_DataJoin_DataJoinCRIDay_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinCRIDay : public AosDataJoin
{
	OmnString					mKeyHead;
	OmnString					mSep;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mIILAsm;

	u64							mCrtMonth;
	OmnString					mCrtPhoneNum;
	u64							mCrtValue;

public:
	AosDataJoinCRIDay();
	AosDataJoinCRIDay(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinCRIDay();

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
						OmnString &phonenum,
						const AosRundataPtr &rdata);
	bool			procCrtValues(const AosRundataPtr &rdata);
	void			clear();
};

#endif

