////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_ReadDocUtil_NormDocOpr_h
#define Aos_DataCube_ReadDocUtil_NormDocOpr_h

#include "DataCube/ReadDocUtil/DocOpr.h"

#include <vector>
using namespace std;

class AosNormDocOpr : public AosDocOpr
{
		
public:
	AosNormDocOpr();
	~AosNormDocOpr();

	virtual bool config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);

	virtual bool shufferDocids(
				const AosRundataPtr &rdata,
				const AosBuffPtr &docids_buff,
				const u32 docid_num,
				map<u32, IdVector> &docid_grp);

	virtual bool shufferDocids(
				const AosRundataPtr &rdata,
				IdVector &docids, 
				map<u32, IdVector> &docid_grp);

	virtual bool sendReadDocTrans(
				const AosRundataPtr &rdata,
				const u64 reqid,
				const u32 svr_id,
				IdVector &docids,
				const AosAsyncRespCallerPtr &resp_caller);

	virtual bool getNextDocidResp(
				const AosBuffPtr &big_buff,
				bool &finished,
				u64 &docid,
				AosBuffPtr &docid_resp);

private:
	bool 	pushDocidToDocidGrp(
				const u64 docid,
				map<u32, IdVector> &docid_grp);

};
#endif

