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
// 2015/01/05	Created by Andy zhang
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/CreateDataCol.h"

#include "API/AosApi.h"
#include "DataCollector/DataCollectorMgr.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosCreateDataCol::AosCreateDataCol(const bool regflag)
:
AosTaskTrans(AosTransType::eCreateDataCol, regflag)
{
}


AosCreateDataCol::AosCreateDataCol(
		const OmnString &datacol_id,
		const u64 &job_id,
		const int svr_id,
		const OmnString &config)
:
AosTaskTrans(AosTransType::eCreateDataCol, svr_id, true, true),
mJobDocid(job_id),
mDataColName(datacol_id),
mConfig(config)
{
}


AosCreateDataCol::~AosCreateDataCol()
{
}


bool
AosCreateDataCol::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mJobDocid = buff->getU64(0);
	mDataColName = buff->getOmnStr("");
	mConfig = buff->getOmnStr("");

	return true;
}


bool
AosCreateDataCol::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mJobDocid);
	buff->setOmnStr(mDataColName);
	buff->setOmnStr(mConfig);
	return true;
}


AosTransPtr
AosCreateDataCol::clone()
{
	return OmnNew AosCreateDataCol(false);
}


bool
AosCreateDataCol::proc()
{
OmnScreen << "create datacol: " << mDataColName << endl;
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, mRdata);
	aos_assert_r(job, false);
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr(); 

	AosXmlParser parser;                                 
	AosXmlTagPtr xml = parser.parse(mConfig, "" AosMemoryCheckerArgs);

	bool rslt = colMgr->createDataCollector(mJobDocid, job, xml, mDataColName, mRdata);
	                                                                                
	aos_assert_r(rslt, false);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	sendResp(resp_buff);
	return true;
}

