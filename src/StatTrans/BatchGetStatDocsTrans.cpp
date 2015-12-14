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
//
// Modification History:
// 2014/01/24	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StatTrans/BatchGetStatDocsTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/StatSvrObj.h"
#include "XmlUtil/XmlTag.h"

AosBatchGetStatDocsTrans::AosBatchGetStatDocsTrans(const bool regflag)
:
AosStatTrans(AosTransType::eBatchGetStatDocs, regflag)
{
}

	
AosBatchGetStatDocsTrans::AosBatchGetStatDocsTrans(
		const u32 cube_id,
		const AosXmlTagPtr &stat_cube_conf,
		vector<u64> &qry_stat_docids,
		const AosXmlTagPtr &stat_qry_conf,
		const u64 reqid,
		const AosAsyncRespCallerPtr &resp_caller)
:
AosStatTrans(AosTransType::eBatchGetStatDocs, cube_id, false, true),
mStatCubeConf(stat_cube_conf),
mQryStatDocids(qry_stat_docids),
mStatQryConf(stat_qry_conf),
mDataScanReqId(reqid),
mRespCaller(resp_caller)
{
}


AosBatchGetStatDocsTrans::~AosBatchGetStatDocsTrans()
{
}


bool
AosBatchGetStatDocsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosStatTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	OmnString conf_str = buff->getOmnStr("");
	aos_assert_r(conf_str != "", false);
	mStatCubeConf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);
	
	u64 qry_docid_num = buff->getU32(0);
	u64 docid;
	for(u32 i=0; i<qry_docid_num; i++)
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		mQryStatDocids.push_back(docid);
	}
	
	conf_str = buff->getOmnStr("");
	mStatQryConf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);

	mDataScanReqId = buff->getU64(0); 
	return true;
}


bool
AosBatchGetStatDocsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosStatTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mStatCubeConf->toString());	

	buff->setU32(mQryStatDocids.size());
	for(u32 i=0; i<mQryStatDocids.size(); i++)
	{
		buff->setU64(mQryStatDocids[i]);
	}
		
	buff->setOmnStr(mStatQryConf->toString());
	
	buff->setU64(mDataScanReqId);
	return true;
}


AosTransPtr
AosBatchGetStatDocsTrans::clone()
{
	return OmnNew AosBatchGetStatDocsTrans(false);
}


bool
AosBatchGetStatDocsTrans::proc()
{
	AosStatSvrObjPtr stat_svr = AosStatSvrObj::getStatSvr();
	if(!stat_svr)	return setErrResp();
	
	u64 t1 = OmnGetTimestamp(); 
	
	AosBuffPtr rslt_buff = stat_svr->retrieve(mRdata, mStatCubeConf,
			mQryStatDocids, mStatQryConf);
	if(!rslt_buff)	return setErrResp();

	OmnScreen << "!!!!!!!! stat svr query time:" << OmnGetTimestamp() - t1 << endl;

	AosBuffPtr resp_buff = OmnNew AosBuff((rslt_buff->dataLen() + 10) AosMemoryCheckerArgs);
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(true);
	resp_buff->setBuff(rslt_buff);
	sendResp(resp_buff);
	return true;
}


bool
AosBatchGetStatDocsTrans::setErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	resp_buff->setU64(mDataScanReqId);
	resp_buff->setU8(false);	
	sendResp(resp_buff);
	return true;
}


bool
AosBatchGetStatDocsTrans::procGetResp()
{
	// Ketty 2013/09/16
	return proc();
}


bool
AosBatchGetStatDocsTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();

	AosTransPtr thisptr(this, false);
	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


