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
#ifndef Aos_DataJoin_DataJoinNetStatusNew_h
#define Aos_DataJoin_DataJoinNetStatusNew_h 

#include "DataJoin/DataJoin.h"


class AosDataJoinNetStatusNew : public AosDataJoin
{
	bool						mNeedCity;
	OmnString					mKeyHead;
	OmnString					mFieldSep;
	OmnString					mSep;

	u64							mCrtDay;
	u64							mFirstDay;
	u64							mLastDay;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mInNetIILAsm;
	AosDataAssemblerObjPtr		mInNetStIILAsm;
	AosDataAssemblerObjPtr		mOutNetIILAsm;
	AosDataAssemblerObjPtr		mOutNetStIILAsm;

	map<OmnString, int>			mInNetStMap;
	map<OmnString, int>			mOutNetStMap;
	
public:
	AosDataJoinNetStatusNew();
	AosDataJoinNetStatusNew(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinNetStatusNew();

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
						u64 &day,
						bool &finish,
						const AosRundataPtr &rdata);
	bool			createMap(
						const u64 &crt_day,
						map<OmnString, int> &crt_map,
						const AosRundataPtr &rdata);
	bool			createSet(
						const u64 &crt_day,
						set<OmnString> &crt_set,
						const AosRundataPtr &rdata);
	bool			addSet(
						map<OmnString, int> &crt_map,
						set<OmnString> &crt_set,
						const AosRundataPtr &rdata);
	bool			removeSet(
						map<OmnString, int> &crt_map,
						set<OmnString> &crt_set,
						const AosRundataPtr &rdata);

	bool			appendInNetEntry(
						const OmnString &key,
						const u64 &day,
						const AosRundataPtr &rdata);
	bool			appendOutNetEntry(
						const OmnString &key,
						const u64 &day,
						const AosRundataPtr &rdata);
	void			clear();
};

#endif

