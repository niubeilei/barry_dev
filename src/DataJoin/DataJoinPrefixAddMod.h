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
// 2013/12/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_DataJoinPrefixAddMod_h
#define Aos_DataJoin_DataJoinPrefixAddMod_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"
#include <map>


class AosDataJoinPrefixAddMod : public AosDataJoin
{
	OmnString					mSep;
	AosIILScannerObjPtr			mIILScanner1;
	AosIILScannerObjPtr			mIILScanner2;
	AosDataAssemblerObjPtr		mIILAsm;

public:
	AosDataJoinPrefixAddMod();
	AosDataJoinPrefixAddMod(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinPrefixAddMod();

	// OmnThreadedShellProc Interface
	virtual	bool	run();
	
	int				getProgress();
	bool 			setQueryContext(
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
	
	void			clear();
};

#endif

