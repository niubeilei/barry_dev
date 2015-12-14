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
// 01/24/2014	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatTrans_BatchGetStatDocsTrans_h
#define Aos_StatTrans_BatchGetStatDocsTrans_h

//#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/StatTimeArea.h"
#include "TransUtil/StatTrans.h"
#include <vector>
using namespace std;

class AosBatchGetStatDocsTrans : public AosStatTrans
{

private:
	AosXmlTagPtr	mStatCubeConf;
	vector<u64>		mQryStatDocids;
	AosXmlTagPtr	mStatQryConf;

	u64				mDataScanReqId;
	AosAsyncRespCallerPtr mRespCaller;
	
public:
	AosBatchGetStatDocsTrans(const bool regflag);
	AosBatchGetStatDocsTrans(
				const u32 cube_id,
				const AosXmlTagPtr &stat_cube_conf,
				vector<u64> &qry_stat_docids,
				const AosXmlTagPtr &stat_qry_conf,
				const u64 reqid,
				const AosAsyncRespCallerPtr &resp_caller);
	~AosBatchGetStatDocsTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool procGetResp();
	
	virtual bool respCallBack();

private:
	bool 	setErrResp();

};
#endif

