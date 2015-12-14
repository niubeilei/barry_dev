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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "LogTrans/AddLogTrans.h"

#include "API/AosApi.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/DocClientObj.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosAddLogTrans::AosAddLogTrans(const bool regflag)
:
AosLogTrans(AosTransType::eAddLog, regflag)
{
}


AosAddLogTrans::AosAddLogTrans(
		const AosRundataPtr &rdata,
		const AosLogOpr::E opr,
		const OmnString &pctr_objid,
		const OmnString &log_name,
		const AosXmlTagPtr &log_info,
		const bool need_save,
		const bool need_resp)
:
AosLogTrans(AosTransType::eAddLog, initCtnrDocid(rdata, pctr_objid), need_save, need_resp),
mOpr(opr),
mPctrObjid(pctr_objid),
mLogName(log_name),
mLogInfo(log_info)
{
	// Set the time created to contents
	u64 crtLogTime = OmnGetSecond();
	mLogInfo->setAttr(AOSTAG_CT_EPOCH, crtLogTime);
	mLogInfo->setAttr(AOSTAG_LOG_CT_EPOCH, crtLogTime);

	if(mLogName == "")
	{
		mLogName = AOSTAG_DFT_LOGNAME;	
	}
}


AosAddLogTrans::~AosAddLogTrans()
{
}


bool
AosAddLogTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosLogTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mOpr = (AosLogOpr::E)buff->getU32(0);
	mCtnrDocid = buff->getU64(0);
	mPctrObjid = buff->getOmnStr("");
	mLogName = buff->getOmnStr("");

	//mLogInfo = buff->getStr("");
	u32 log_len = buff->getU32(0); 
	AosBuffPtr log_buff = buff->getBuff(log_len, false AosMemoryCheckerArgs); 
	AosXmlParser parser;
	mLogInfo = parser.parse(log_buff->data(),
			log_len, "" AosMemoryCheckerArgs);

	aos_assert_r(mLogInfo && mCtnrDocid, false);
	return true;
}


bool
AosAddLogTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosLogTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mOpr);
	buff->setU64(mCtnrDocid);
	buff->setOmnStr(mPctrObjid);
	buff->setOmnStr(mLogName);

	buff->setU32(mLogInfo->getDataLength());
	buff->setBuff((char *)mLogInfo->getData(), mLogInfo->getDataLength());
	return true;
}


AosTransPtr
AosAddLogTrans::clone()
{
	return OmnNew AosAddLogTrans(false);
}


bool
AosAddLogTrans::procLog()
{
	u64 logid;
	bool rslt = AosPhyLogSvr::getSelf()->addLog(
		mOpr, mCtnrDocid, mPctrObjid, mLogName, mLogInfo,logid, mRdata);
	
	if(!isNeedResp())	return true;

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(logid);
	
	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);

	return true;
}


u64
AosAddLogTrans::initCtnrDocid(
		const AosRundataPtr &rdata,
		const OmnString &pctr_objid)
{
	if (pctr_objid == AOSOBJIDPRE_ROOTCTNR || pctr_objid == AOSSTYPE_SYSACCT)
	{
		mCtnrDocid = AOSDOCID_ROOTCTNR;
	}
	else
	{
		mCtnrDocid = AosDocClientObj::getDocClient()->getDocidByObjid(pctr_objid, rdata);
	}

	aos_assert_r(mCtnrDocid> 0, 0);
	return mCtnrDocid;
}

