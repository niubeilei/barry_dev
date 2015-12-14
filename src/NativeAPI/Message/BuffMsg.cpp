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
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Message/BuffMsg.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/DataTypes.h"

int AosBuffMsg::smVersion = 10;
OmnMutexPtr AosBuffMsg::smLock = OmnNew OmnMutex();
vector<AosBuffMsgPtr> AosBuffMsg::smMsgs;

AosBuffMsg::AosBuffMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const int send_phyid, 
		const int recv_phyid, 
		const bool is_request,
		const int version)
:
mMsgId(msgid),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
	if (!OmnMsgId::isValid(msgid))
	{
		OmnString errmsg = "Invalid msgid: ";
		errmsg << msgid;
		OmnThrowException(errmsg);
		return;
	}

	setMsgHead(msgid, is_request);
	setFieldU32(AosMsgFieldId::eMsgId, msgid);
	setFieldInt(AosMsgFieldId::eSendPhysicalId, send_phyid);
	setFieldInt(AosMsgFieldId::eRecvPhysicalId, recv_phyid);
	setFieldInt(AosMsgFieldId::eMsgVersion, version);
	if (rdata) rdata->setForSending(*this);
}


AosBuffMsg::AosBuffMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const u32 send_cubeid, 
		const u32 recv_cubeid, 
		const bool is_request,
		const int version)
:
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
	if (!OmnMsgId::isValid(msgid))
	{
		OmnString errmsg = "Invalid msgid: ";
		errmsg << msgid;
		OmnThrowException(errmsg);
		return;
	}

	setMsgHead(msgid, is_request);
	setFieldInt(AosMsgFieldId::eSendCubeId, send_cubeid);
	setFieldInt(AosMsgFieldId::eRecvCubeId, recv_cubeid);
	setFieldU32(AosMsgFieldId::eMsgId, msgid);
	setFieldInt(AosMsgFieldId::eMsgVersion, version);
	if (rdata) rdata->setForSending(*this);
}


AosBuffMsg::AosBuffMsg(const AosBuffPtr &buff)
:
mBuff(buff)
{
	if (!parseMsg())
	{
		OmnThrowException("Invalid Message");
		return;
	}
}


AosBuffMsg::~AosBuffMsg()
{
}


bool
AosBuffMsg::setMsgHead(
		const OmnMsgId::E msgid, 
		const bool is_request)
{
	aos_assert_r(mBuff, false);
	mBuff->setU32(eBuffMsgPoison1);
	mBuff->setU32(eBuffMsgPoison2);
	mBuff->setU32(OmnMsgId::eBuffMsg);
	mBuff->setU32(msgid);
	mBuff->setChar(is_request);
	return true;
}


bool
AosBuffMsg::verifyMsgHead(
		const AosBuffPtr &buff,
		OmnMsgId::E &msgid, 
		bool &is_request)
{
	aos_assert_r(buff, false);
	u32 poison1 = buff->getU32(0);
	u32 poison2 = buff->getU32(0);
	OmnMsgId::E msgtype = (OmnMsgId::E)buff->getU32(0);
	if (!(poison1 == eBuffMsgPoison1 &&
			poison2 == eBuffMsgPoison2 &&
			msgtype == OmnMsgId::eBuffMsg)) return false;
	msgid = (OmnMsgId::E)buff->getU32(OmnMsgId::eInvalidMsgId);
	is_request = buff->getChar(1);
	return OmnMsgId::isValid(msgid);
}


AosBuffMsgPtr
AosBuffMsg::createMsg(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnMsgId::E msgid;
	bool is_request;
	bool rslt = verifyMsgHead(buff, msgid, is_request);
	aos_assert_rr(rslt, rdata, 0);
	aos_assert_rr(OmnMsgId::isValid(msgid), rdata, 0);
	aos_assert_rr(msgid < smMsgs.size(), rdata, 0);

	AosBuffMsgPtr msg = smMsgs[msgid];
	aos_assert_rr(msg, rdata, 0);
	return msg->clone(rdata, buff);
}


bool
AosBuffMsg::registerMsg(const AosBuffMsgPtr &msg)
{
	aos_assert_r(msg, false);
	smLock->lock();
	OmnMsgId::E msgid = msg->getMsgId();
	aos_assert_rl(OmnMsgId::isValid(msgid), smLock, false);
	if (msgid >= smMsgs.size()) smMsgs.resize(msgid);
	aos_assert_rl(!smMsgs[msgid], smLock, false);
	smMsgs[msgid] = msg;
	smLock->unlock();
	return true;
}


AosRundataPtr
AosBuffMsg::getRundata() 
{
	return AosRundata::serializeFromMsg(*this AosMemoryCheckerArgs);
}


bool 
AosBuffMsg::setFieldU64(const AosMsgFieldId::E field, const u64 &value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		u64
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU64);
	mBuff->setU64(value);
	return true;
}


bool 
AosBuffMsg::setFieldU32(const AosMsgFieldId::E field, const u32 value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		u32
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU32);
	mBuff->setU32(value);
	return true;
}


bool 
AosBuffMsg::setFieldU16(const AosMsgFieldId::E field, const u16 value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		u16
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU16);
	mBuff->setU16(value);
	return true;
}


bool 
AosBuffMsg::setFieldU8(const AosMsgFieldId::E field, const u8 value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		u8
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU8);
	mBuff->setU8(value);
	return true;
}


bool 
AosBuffMsg::setFieldInt64(const AosMsgFieldId::E field, const int64_t &value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		int64_t
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eInt64);
	mBuff->setI64(value);
	return true;
}


bool 
AosBuffMsg::setFieldInt(const AosMsgFieldId::E field, const int value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		int
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eInt32);
	mBuff->setInt(value);
	return true;
}


bool 
AosBuffMsg::setFieldInt16(const AosMsgFieldId::E field, const int16_t value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		int16_t
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eInt16);
	mBuff->setI16(value);
	return true;
}


bool 
AosBuffMsg::setFieldChar(const AosMsgFieldId::E field, const char value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		char	
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eChar);
	mBuff->setChar(value);
	return true;
}


bool 
AosBuffMsg::setFieldFloat(const AosMsgFieldId::E field, const float &value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		float
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eFloat);
	mBuff->setFloat(value);
	return true;
}


bool 
AosBuffMsg::setFieldDouble(const AosMsgFieldId::E field, const double &value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		double
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eDouble);
	mBuff->setDouble(value);
	return true;
}


bool 
AosBuffMsg::setFieldStr(const AosMsgFieldId::E field, const OmnString &value)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		eString
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eString);
	mBuff->setOmnStr(value);
	return true;
}


bool 
AosBuffMsg::setFieldBuff(const AosMsgFieldId::E field, const AosBuffPtr &buff)
{
	// Format:
	// 		field		u16
	// 		typ			u8
	// 		value		AosBuff	
	// aos_assert_r(mBuff, false);
	// mBuff->setU16(field);
	// mBuff->setU8(AosDataType::eBuff);
	// mBuff->setBuff(buff);
	return true;
}


u64 
AosBuffMsg::getFieldU64(const AosMsgFieldId::E field, const u64 &dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU64, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU64(dft);
}


u32 
AosBuffMsg::getFieldU32(const AosMsgFieldId::E field, const u32 dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU32, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU32(dft);
}


u16
AosBuffMsg::getFieldU16(const AosMsgFieldId::E field, const u16 dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU16, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU16(dft);
}


u8
AosBuffMsg::getFieldU8(const AosMsgFieldId::E field, const u8 dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU8, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU8(dft);
}


int64_t
AosBuffMsg::getFieldInt64(const AosMsgFieldId::E field, const int64_t &dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eInt64, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getI64(dft);
}


int 
AosBuffMsg::getFieldInt(const AosMsgFieldId::E field, const int dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eInt32, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getInt(dft);
}


int16_t 
AosBuffMsg::getFieldInt16(const AosMsgFieldId::E field, const int16_t dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eInt16, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getI16(dft);
}


char
AosBuffMsg::getFieldChar(const AosMsgFieldId::E field, const char dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eChar, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getChar(dft);
}


OmnString
AosBuffMsg::getFieldStr(const AosMsgFieldId::E field, const OmnString &dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eString, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getOmnStr(dft);
}


AosBuffPtr
AosBuffMsg::getFieldBuff(const AosMsgFieldId::E field, const bool copy)
{
	aos_assert_r(mBuff, 0);
	aos_assert_r(field < mFieldIdx.size(), 0);
	aos_assert_r(mFieldType[field] == AosDataType::eString, 0);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getBuffAsBinary(copy AosMemoryCheckerArgs);
}


bool
AosBuffMsg::parseMsg()
{
	// Buff message is in the format:
	// 		poison1			u32
	// 		poison2			u32
	// 		msgtype			u32
	// 		version			u32
	// 		field_id		u16
	// 		data type		u8
	// 		length			u32		(optional)
	// 		message body	variable
	// 		field_id		u16
	// 		data type		u8
	// 		length			u32		(optional)
	// 		message body	variable
	// 		...
	
	aos_assert_r(verifyMsgHead(mBuff, mMsgId, mIsRequest), false);
	aos_assert_r(OmnMsgId::isValid(mMsgId), false);

	AosMsgFieldId::E field_id;
	mFieldIdx.resize(AosMsgFieldId::getNumFields());
	while ((field_id = (AosMsgFieldId::E)mBuff->getU16(0)) != 0)
	{
		aos_assert_r(AosMsgFieldId::isValid(field_id), false);
		int64_t idx = mBuff->getCrtIdx();
		AosDataType::E type = (AosDataType::E)mBuff->getU8(0);
		aos_assert_r(AosDataType::isValid(type), false);

		int len = AosBuff::getDataSize(type);
		if (len <= 0)
		{
			if (type == AosDataType::eString)
			{
				// The format is:
				// 	field		u16
				// 	type		u8
				//	length		int
				//	value		variable
				int nn = mBuff->getInt(-1);
				aos_assert_r(nn >= 0, false);
				len += AosBuff::getStrSize(nn);
			}
			else
			{
				// It is variable
				len = mBuff->getInt(-1);
				idx += sizeof(u8) + sizeof(u32);
			}
		}
		aos_assert_r(len > 0, 0);

		mFieldIdx[field_id] = idx;
		mFieldType[field_id] = type;
		idx += len;
	}

	return true;
}


bool 
AosBuffMsg::setFieldU64(const OmnString &name, const u64 &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU64);
	mBuff->setOmnStr(name);
	mBuff->setU64(value);
	return true;
}


bool 
AosBuffMsg::setFieldU32(const OmnString &name, const u32 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU32);
	mBuff->setOmnStr(name);
	mBuff->setU32(value);
	return true;
}


bool 
AosBuffMsg::setFieldU16(const OmnString &name, const u16 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU16);
	mBuff->setOmnStr(name);
	mBuff->setU16(value);
	return true;
}


bool 
AosBuffMsg::setFieldU8(const OmnString &name, const u8 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU8);
	mBuff->setOmnStr(name);
	mBuff->setU8(value);
	return true;
}


bool 
AosBuffMsg::setFieldInt64(const OmnString &name, const int64_t &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eInt64);
	mBuff->setOmnStr(name);
	mBuff->setI64(value);
	return true;
}


bool 
AosBuffMsg::setFieldInt(const OmnString &name, const int value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eInt32);
	mBuff->setOmnStr(name);
	mBuff->setInt(value);
	return true;
}


bool 
AosBuffMsg::setFieldInt16(const OmnString &name, const int16_t value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eInt16);
	mBuff->setOmnStr(name);
	mBuff->setI16(value);
	return true;
}


bool 
AosBuffMsg::setFieldChar(const OmnString &name, const char value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eChar);
	mBuff->setOmnStr(name);
	mBuff->setChar(value);
	return true;
}


bool 
AosBuffMsg::setFieldFloat(const OmnString &name, const float &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eFloat);
	mBuff->setOmnStr(name);
	mBuff->setFloat(value);
	return true;
}


bool 
AosBuffMsg::setFieldDouble(const OmnString &name, const double &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eDouble);
	mBuff->setOmnStr(name);
	mBuff->setDouble(value);
	return true;
}


bool 
AosBuffMsg::setFieldStr(const OmnString &name, const OmnString &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eString);
	mBuff->setOmnStr(name);
	mBuff->setOmnStr(value);
	return true;
}


bool 
AosBuffMsg::setFieldBuff(const OmnString &name, const AosBuffPtr &buff)
{
	aos_assert_r(mBuff, false);
	// mBuff->setU8(AosDataType::eBuff);
	// mBuff->setOmnStr(name);
	// mBuff->setBuff(buff);
	return true;
}


u64 	
AosBuffMsg::getFieldU64(const OmnString &name, const u64 &dft)
{
	// aos_assert_r(name != "", false);
	// mapitr_t itr = mFieldMap.find(name);
	// if (itr == mFieldMap.end()) return dft;
	// return itr->second;
	OmnNotImplementedYet;
	return dft;
}


u32 	
AosBuffMsg::getFieldU32(const OmnString &name, const u32 dft)
{
	OmnNotImplementedYet;
	return dft;
}


u16 	
AosBuffMsg::getFieldU16(const OmnString &name, const u16 dft)
{
	OmnNotImplementedYet;
	return dft;
}


u8  	
AosBuffMsg::getFieldU8(const OmnString &name, const u8 dft)
{
	OmnNotImplementedYet;
	return dft;
}


int64_t 
AosBuffMsg::getFieldInt64(const OmnString &name, const int64_t &dft)
{
	OmnNotImplementedYet;
	return dft;
}


int 	
AosBuffMsg::getFieldInt(const OmnString &name, const int dft)
{
	OmnNotImplementedYet;
	return dft;
}


int16_t 
AosBuffMsg::getFieldInt16(const OmnString &name, const int16_t dft)
{
	OmnNotImplementedYet;
	return dft;
}


char 	
AosBuffMsg::getFieldChar(const OmnString &name, const char dft)
{
	OmnNotImplementedYet;
	return dft;
}


float 	
AosBuffMsg::getFieldFloat(const OmnString &name, const float dft)
{
	OmnNotImplementedYet;
	return dft;
}


double	
AosBuffMsg::getFieldDouble(const OmnString &name, const double &dft)
{
	OmnNotImplementedYet;
	return dft;
}


OmnString 
AosBuffMsg::getFieldStr(const OmnString &name, const OmnString &dft)
{
	OmnNotImplementedYet;
	return dft;
}


AosBuffPtr 
AosBuffMsg::getFieldBuff(const OmnString &name, const bool copy)
{
	OmnNotImplementedYet;
	return 0;
}

