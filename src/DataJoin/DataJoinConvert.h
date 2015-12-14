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
#ifndef Aos_DataJoin_DataJoinConvert_h
#define Aos_DataJoin_DataJoinConvert_h 

#include "DataJoin/DataJoin.h"


class AosDataJoinConvert : public AosDataJoin
{
	enum ConvertType
	{
		eInvalid,

		eIgnoreLastDigitalStr,
		eUnicomNetStatus,

		eMax
	};

	ConvertType					mType;
	OmnString					mSep;

	// IgnoreLastDigitalStr
	int							mConvertIdx;
	OmnString					mConvertSep;
	//OmnString					mSep;

	// UnicomNetStatus
	//OmnString					mSep;

	AosIILScannerObjPtr			mIILScanner;
	AosDataAssemblerObjPtr		mIILAsm;

public:
	AosDataJoinConvert();
	AosDataJoinConvert(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinConvert();

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
						OmnString &key,
						u64 &value,
						bool &finish,
						const AosRundataPtr &rdata);
	void			clear();

	bool			procIgnoreLastDigitalStr(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata);

	bool			procUnicomNetStatus(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata);
};

#endif

