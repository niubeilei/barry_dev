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
#ifndef Aos_DataJoin_DataJoinCpnTownCode_h
#define Aos_DataJoin_DataJoinCpnTownCode_h 

#include "DataJoin/DataJoin.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataJoinCpnTownCode : public AosDataJoin
{
	bool					mHaveStatisticHead;
	int						mKeyHead;
	OmnString				mSep;

	AosIILEntryMapPtr		mCpnCityCodeMap;
	AosIILEntryMapPtr		mUpnCityCodeMap;
	AosIILEntryMapPtr		mUpnTownCodeMap;
	
	OmnString				mCrtCpn;
	u64						mCrtCDRsNum;
	u64						mCrtTownCode;

	AosIILScannerObjPtr		mIILScanner;
	AosDataAssemblerObjPtr	mIILAssembler;
	
public:
	AosDataJoinCpnTownCode();
	AosDataJoinCpnTownCode(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinCpnTownCode();

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
	bool			runJoin(AosRundata *rdata);
	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	
	bool			getNextKey(
						OmnString &cpn,
						OmnString &upn,
						u64 &value,
						bool &finish,
						AosRundata *rdata);
	bool			procNextKey(
						const OmnString &cpn,
						const OmnString &upn,
						const u64 &value,
						AosRundata *rdata);
	bool			procEnd(AosRundata *rdata);
	bool			appendEntry(
						const OmnString &key,
						const u64 &value,
						AosRundata *rdata);
	int				getCpnCityCode(
						const OmnString &cpn,
						AosRundata *rdata);
	int				getUpnCityCode(
						const OmnString &upn,
						AosRundata *rdata);
	int				getUpnTownCode(
						const OmnString &upn,
						AosRundata *rdata);
	void			clear();
};

#endif

