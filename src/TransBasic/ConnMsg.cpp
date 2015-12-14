////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 05/08/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransBasic/ConnMsg.h"

#include "API/AosApiG.h"
#include "TransBasic/AppMsg.h"

static AosConnMsgPtr  sgMsg[MsgType::eMax];
	
AosConnMsg::AosConnMsg(
		const MsgType::E tp,
		const bool reg_flag)
:
mType(tp)
{
	if(reg_flag)
	{
		AosConnMsgPtr thisptr(this, false);
		registerMsg(tp, thisptr);
	}
}


AosConnMsg::~AosConnMsg()
{
}


bool
AosConnMsg::registerMsg(const MsgType::E tp, const AosConnMsgPtr &msg)
{
	//if(!sgLvMsg[lv])
	//{
	//	sgLvMsg[lv] = msg;
	//}
	aos_assert_r(MsgType::isValid(tp), false);
	if(!sgMsg[tp])
	{
		sgMsg[tp] = msg;
	}
	return true;
}


AosConnMsgPtr
AosConnMsg::serializeFromStatic(const AosBuffPtr &buff, const bool set_conn_buff)
{
	// for SvrProxy serialize from.	
	AosConnMsgPtr msg;

	MsgType::E tp = (MsgType::E)buff->getU16(0);
	u32 beg_idx = buff->getCrtIdx(); // beg_idx. will ignore tp. serializeFrom don't need tp.

if (!MsgType::isValid(tp))
{
OmnScreen << tp << endl;
}
	aos_assert_r(MsgType::isValid(tp), NULL);

	if(sgMsg[tp])
	{
		msg = sgMsg[tp]->clone2();
		msg->serializeFrom(buff);
	}
	else
	{
		bool is_app_msg = buff->getU8(0);
		aos_assert_r(is_app_msg, 0);
	
		msg = AosAppMsg::serializeStatic(buff, beg_idx, tp);
	}

	//if(beg_idx == 0) msg->mConnBuff = buff;
	if(set_conn_buff)	msg->mConnBuff = buff;
	return msg;	
}

/*
AosConnMsgPtr
AosConnMsg::serializeFromStatic(const AosBuffPtr &buff, const bool set_conn_buff)
{
	//bool set_conn_buff = (buff->getCrtIdx() == 0);
	MsgType::E tp = (MsgType::E)buff->getU16(0);
	aos_assert_r(sgMsg[tp], 0);

	AosConnMsgPtr msg = sgMsg[tp]->clone2();
	msg->serializeFrom(buff);
	
	//if(set_conn_buff) msg->mConnBuff = buff;
	if(set_conn_buff)	msg->mConnBuff = buff;
	return msg;
}
*/

AosBuffPtr
AosConnMsg::getConnBuff()
{
	if(!mConnBuff)
	{
		mConnBuff = OmnNew AosBuff(100, 0 AosMemoryCheckerArgs);
		bool rslt = serializeTo(mConnBuff);
		aos_assert_r(rslt, 0);

		// add by andy for CRC
		/*
		u32 parity_value = AosGenerateParity(mConnBuff->data(), mConnBuff->dataLen());
		u32 parity_value1 = AosGenerateParity(mConnBuff->data(), mConnBuff->dataLen());
		aos_assert_r(parity_value1 == parity_value, NULL);
		u32 data_len =  mConnBuff->dataLen();
		aos_assert_r(parity_value <= mConnBuff->dataLen(), mConnBuff);
		mConnBuff->addBuff((char *)(&parity_value), sizeof(u32));
		mConnBuff->addBuff((char *)(&data_len), sizeof(u32));
		mConnBuff->reset();
		*/
	}

	return mConnBuff;
}


u64
AosConnMsg::getSize()
{
	AosBuffPtr conn_buff = getConnBuff();
	aos_assert_r(conn_buff, false);
	return conn_buff->dataLen();
}


bool
AosConnMsg::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	// add by andy for CRC
	/*
	i64 crt_idx = buff->getCrtIdx();	
	u32 parity_value = AosGenerateParity(buff->data(), buff->dataLen()-sizeof(u32) * 2);
	u32 tmp_value = *(u32 *)(buff->data() + buff->dataLen()-sizeof(u32)*2);
	u32 data_len = *(u32 *)(buff->data() + buff->dataLen()-sizeof(u32));
	if (parity_value != tmp_value)
	{
		OmnAlarm << "error" << enderr;
	}
	buff->setCrtIdx(crt_idx);
	*/

	//mType = (MsgType::E)buff->getU8(0);
	bool is_app_msg = buff->getU8(0);
	aos_assert_r(is_app_msg == isAppMsg(), false);
	return true;
}


bool
AosConnMsg::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	buff->setU16(mType);
	buff->setU8(isAppMsg());		// for Svrproxy serialize
	return true;
}

AosConnMsgPtr
AosConnMsg::copy()
{
	AosBuffPtr old_buff = getConnBuff();
	aos_assert_r(old_buff, 0);

	AosBuffPtr new_buff = OmnNew AosBuff(old_buff->dataLen(),
			0 AosMemoryCheckerArgs); 
	new_buff->setBuff(old_buff);
	new_buff->setCrtIdx(0);

	AosConnMsgPtr new_msg = serializeFromStatic(new_buff, true);
	return new_msg;
}

OmnString
AosConnMsg::getStrType()
{
	OmnString str_tp;
	str_tp << mType;
	return str_tp;
}

bool
AosConnMsg::registerDynMsg(const MsgType::E tp, const AosConnMsgPtr &msg)
{
	aos_assert_r(MsgType::isValid(tp), false);

	sgMsg[tp] = msg;
	return true;
}
