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
// 11/16/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/DataAssemblerBuff.h"

#include "API/AosApiG.h"
#include "API/AosApiS.h"
#include "JobTrans/CreateDataReceiver.h"
#include "JobTrans/DataSender.h"


AosDataAssemblerBuff::AosDataAssemblerBuff(
		const OmnString &asm_key,
		const u64 task_docid) 
:
AosDataAssembler(AosDataAssemblerType::eBuff, AOSDATAASSEMBLER_BUFF, asm_key, task_docid),
mBuffMaxSizeToSend(AosDataAssembler::sgDftBuffLen),
mAppendDataLen(0),
mSendDataLen(0),
mEntryNum(0),
mRecordType(AosDataRecordType::eInvalid)
{
}


AosDataAssemblerBuff::~AosDataAssemblerBuff()
{
}


bool
AosDataAssemblerBuff::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataAssembler::config(def, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr datarecord = def->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);
	datarecord = datarecord->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);
	mRecordType = AosDataRecordType::toEnum(datarecord->getAttrStr("type", "buff"));
	aos_assert_r(mRecordType != AosDataRecordType::eInvalid, false);

	mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	mBuffMaxSizeToSend = def->getAttrInt64(AOSTAG_MAX_BUFFSIZE, -1);
	if (mBuffMaxSizeToSend <= 0) mBuffMaxSizeToSend = AosDataAssembler::sgDftBuffLen;

	return true;
}


AosDataAssemblerObjPtr
AosDataAssemblerBuff::createAssemblerBuff(
		const OmnString &asm_key,
		const u64 task_docid,
		const AosXmlTagPtr &def, 
		AosRundata *rdata)
{
	aos_assert_r(def, 0);

	AosDataAssemblerBuff * asm_buff = OmnNew AosDataAssemblerBuff(asm_key, task_docid);
	aos_assert_r(asm_buff, NULL);
	bool rslt = asm_buff->config(def, rdata);
	aos_assert_r(rslt, 0);
	return asm_buff;
}
	

bool
AosDataAssemblerBuff::sendStart(AosRundata *rdata)
{
	OmnString conf = "<datareceiver ";
	conf << AOSTAG_ZKY_TYPE << "=\"datareceiver\">" 
			   << "</datareceiver>";

	u32 svr_id = AosGetSelfServerId();
	AosTransPtr trans = OmnNew AosCreateDataReceiver(mAsmKey, conf, svr_id);
	aos_assert_r(trans, false);

	AosBuffPtr resp;
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);
	aos_assert_r(!timeout, false);
	aos_assert_r(resp && resp->dataLen() >= 0, false);
	aos_assert_r(resp->getBool(false), false);
	return true;
}

	
bool
AosDataAssemblerBuff::sendFinish(AosRundata *rdata)
{
	mLockRaw->lock();
	bool rslt = sendPriv(rdata);
	aos_assert_rl(rslt, mLockRaw, false);
	
	OmnScreen << "str attr, mAsmKey:" << mAsmKey << ", "
			  << "mAppendDataLen: " << mAppendDataLen << ", "
			  << "mSendDataLen: " << mSendDataLen << ", "
			  << "mEntryNum: " << mEntryNum << endl;
	mLockRaw->unlock();

	return true;	
}


bool 
AosDataAssemblerBuff::waitFinish(AosRundata *rdata)
{
	return true;	
}


bool
AosDataAssemblerBuff::appendEntry(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	mLockRaw->lock();
	bool rslt = false;
	AosDataRecordType::E type = record->getType();
	aos_assert_r(type == mRecordType, false);

	const char *data = record->getData(rdata);
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);
	mAppendDataLen += rcd_len;
	mEntryNum++;

	//1.record type:buff
	//format: rcd_len + data
	if (mRecordType == AosDataRecordType::eBuff)               
	{
		rslt = mBuff->setEncodeCharStr(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	//2.record type:fixbin
	//format:data
	else if (mRecordType == AosDataRecordType::eFixedBinary || mRecordType == AosDataRecordType::eCSV)
	{
		rslt = mBuff->setBuff(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	else                            
	{                               
		OmnNotImplementedYet;       
	}                               

	i64 len = mBuff->dataLen();
	if (len >= mBuffMaxSizeToSend)
	{
		bool rslt = sendPriv(rdata);
		aos_assert_rl(rslt, mLockRaw, false);
	}
	mLockRaw->unlock();
	return true;
}


bool
AosDataAssemblerBuff::sendPriv(AosRundata* rdata)
{
	aos_assert_r(mBuff, false);
	i64 len = mBuff->dataLen();
	if (len > 0)
	{
		mSendDataLen += len;
		sendBuff(rdata);
		mBuff->reset();
		mBuff->setDataLen(0);
	}
	return true;
}


bool
AosDataAssemblerBuff::sendBuff(
		AosRundata *rdata)
{
	u32 svr_id = AosGetSelfServerId();

	AosTransPtr trans = OmnNew AosDataSender(mAsmKey, mBuff, svr_id);
	aos_assert_r(trans, false);

	AosBuffPtr resp;
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);
	aos_assert_r(!timeout, false);
	aos_assert_r(resp && resp->dataLen() >= 0, false);
	aos_assert_r(resp->getBool(false), false);
	return true;
}
