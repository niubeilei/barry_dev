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
#ifndef Aos_DataJoin_DataJoinCRI_h
#define Aos_DataJoin_DataJoinCRI_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinCRI : public AosDataJoin
{
	OmnString					mKeyHead;
	OmnString					mKeyHead2;
	OmnString					mSep;

	AosIILEntryMapPtr			mCallDurMap;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mIILAsm;

	u64							mCrtMonth;
	OmnString					mCrtPhoneNum;
	u64							mCrtValue;
	vector<OmnString>			mRegions;

public:
	AosDataJoinCRI();
	AosDataJoinCRI(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinCRI();

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
						OmnString &region,
						u64 &value,
						const AosRundataPtr &rdata);
	bool			procCrtValues(const AosRundataPtr &rdata);
	bool			addAsmEntry(
						const OmnString &region,
						const AosRundataPtr &rdata);
	void			clear();
};

#endif

