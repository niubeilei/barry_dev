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
#include "JobTrans/GetFileListByDataCol.h"

#include "API/AosApi.h"
#include "DataCollector/DataCollectorMgr.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosGetFileListByDataCol::AosGetFileListByDataCol(const bool regflag)
:
AosTaskTrans(AosTransType::eGetFileListByDataCol, regflag)
{
}


AosGetFileListByDataCol::AosGetFileListByDataCol(
		const OmnString &datacol_id,
		const u64 &job_id,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetFileListByDataCol, svr_id, false, true),
mDataColId(datacol_id),
mJobDocid(job_id)
{
}


AosGetFileListByDataCol::~AosGetFileListByDataCol()
{
}


bool
AosGetFileListByDataCol::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mDataColId = buff->getOmnStr("");
	mJobDocid = buff->getU64(0);
	return true;
}


bool
AosGetFileListByDataCol::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mDataColId);
	buff->setU64(mJobDocid);
	return true;
}


AosTransPtr
AosGetFileListByDataCol::clone()
{
	return OmnNew AosGetFileListByDataCol(false);
}


bool
AosGetFileListByDataCol::proc()
{
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, mRdata);
	aos_assert_r(job, false);
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr(); 
	                                                                                
	vector<AosXmlTagPtr> files;
	bool rslt = colMgr->getOutputFiles(mDataColId, files, mRdata);
	aos_assert_r(rslt, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	OmnString str;
	str << "<files>";
	for (u32 i=0; i<files.size(); i++)
	{
		str << files.at(i)->toString();
	}
	str << "</files>";
	resp_buff->setOmnStr(str);
	sendResp(resp_buff);
	return true;
}

