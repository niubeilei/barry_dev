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
// 2014/07/21	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StatTrans/BatchSaveStatDocsTrans.h"

#include "SEInterfaces/StatSvrObj.h"

AosBatchSaveStatDocsTrans::AosBatchSaveStatDocsTrans(const bool regflag)
:
AosStatTrans(AosTransType::eBatchSaveStatDocs, regflag)
{
}


AosBatchSaveStatDocsTrans::AosBatchSaveStatDocsTrans(
		const u32 cube_id,
		const AosXmlTagPtr &stat_cube_conf,
		const AosBuffPtr &input_data,
		AosStatModifyInfo &stat_mdf_info)
:
AosStatTrans(AosTransType::eBatchSaveStatDocs, cube_id, false, true),
mStatCubeConf(stat_cube_conf),
mInputData(input_data),
mStatMdfInfo(stat_mdf_info),
mRecordBuff2Conf(NULL)//yang
{
}



AosBatchSaveStatDocsTrans::~AosBatchSaveStatDocsTrans()
{
}


bool
AosBatchSaveStatDocsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosStatTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	OmnString conf_str = buff->getOmnStr("");
	aos_assert_r(conf_str != "", false);
	mStatCubeConf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);

	/*
	//yang
	conf_str = buff->getOmnStr("");
	mRecordBuff2Conf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);
	*/

	u32	data_len = buff->getU32(0);
	aos_assert_r(data_len, false);
	mInputData = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	mStatMdfInfo.serializeFrom(mRdata, buff);
	


	/*
	//create datarecord
	mRecordBuff2 = AosDataRecordObj::createDataRecordStatic(record_doc,
			NULL, rdata AosMemoryCheckerArgs);
	*/

	return true;
}


bool
AosBatchSaveStatDocsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosStatTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mStatCubeConf->toString());	
	//buff->setOmnStr(mRecordBuff2Conf->toString());//yang
	buff->setU32(mInputData->dataLen());
	buff->setBuff(mInputData);
	mStatMdfInfo.serializeTo(mRdata, buff);	

	return true;
}


AosTransPtr
AosBatchSaveStatDocsTrans::clone()
{
	return OmnNew AosBatchSaveStatDocsTrans(false);
}


bool
AosBatchSaveStatDocsTrans::proc()
{
	AosStatSvrObjPtr stat_svr = AosStatSvrObj::getStatSvr();
	if(!stat_svr)	return false;	
	//if(!stat_svr)	return setErrResp();

	bool rslt = stat_svr->modify(mRdata, mStatCubeConf, mInputData, mStatMdfInfo);
	aos_assert_r(rslt, false);
	if(!rslt)	return setErrResp();

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	sendResp(resp_buff);
	return true;
}


bool
AosBatchSaveStatDocsTrans::setErrResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	resp_buff->setU8(false);	
	sendResp(resp_buff);
	return true;
}
