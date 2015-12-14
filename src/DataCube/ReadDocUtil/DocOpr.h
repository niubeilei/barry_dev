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
#ifndef Aos_DataCube_ReadDocUtil_DocOpr_h
#define Aos_DataCube_ReadDocUtil_DocOpr_h

#include "DataCube/ReadDocUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlDoc.h"
#include "Util/Buff.h"

#include <vector>
#include <map>
using namespace std;

typedef vector<u64> IdVector;

class AosDocOpr : public OmnRCObject 
{
	OmnDefineRCObject;
	
	enum
	{
		//eEachGroupDocidNum = 1000,
		eEachGroupDocidNum = 100000,
	};
	
public:
	static AosDocOprPtr getDocOpr(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	static void include();
	
	virtual bool config(
				const AosRundataPtr &rdata,	
				const AosXmlTagPtr &conf);
	
	virtual u64 getEachGroupDocidNum(const u64 total_num);

	virtual bool shufferDocids(
				const AosRundataPtr &rdata,
				const AosBuffPtr &docids_buff,
				const u32 docid_num,
				map<u32, IdVector> &docid_grp) = 0;

	virtual bool shufferDocids(
				const AosRundataPtr &rdata,
				IdVector &docids, 
				map<u32, IdVector> &docid_grp) = 0;

	virtual bool sendReadDocTrans(
				const AosRundataPtr &rdata,
				const u64 reqid,
				const u32 svr_id,
				IdVector &docids,
				const AosAsyncRespCallerPtr &resp_caller) = 0;

	virtual bool getNextDocidResp(
				const AosBuffPtr &big_buff,
				bool &finished,
				u64 &docid,
				AosBuffPtr &docid_resp) = 0;
	
	virtual AosBuffPtr getMetaResp(const AosBuffPtr &big_buff){ return 0; };

	// Ken Lee, 2015/01/06
	virtual AosBuffPtr getAllResp(
				const AosBuffPtr metaResp,
				IdVector &totalDocids,
				IdVector &schemaids,
				map<u64, AosBuffPtr> &allResps);

};
#endif

