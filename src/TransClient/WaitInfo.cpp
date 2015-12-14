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
// 2013/07/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransClient/WaitInfo.h"

#include "TransBasic/Trans.h"

AosWaitInfo::AosWaitInfo(const AosTransPtr &trans, const bool show_log)
:
mTrans(trans),
mWaitTime(0),
mShowLog(show_log)
{
	aos_assert(trans);
	mNeedAckNum = trans->getNeedAckNum();

	if(mShowLog)
	{
		OmnScreen << "new wait_info:"
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; need_ack_num:" << mNeedAckNum 
			<< endl;
	}
}


void 
AosWaitInfo::addAck(const int from_sid)
{
	set<int>::iterator itr = mDeathSids.find(from_sid);
	if(itr != mDeathSids.end())	mDeathSids.erase(itr);

	mAckSids.insert(from_sid); 
}


void
AosWaitInfo::addDeathSvr(const int death_sid)
{
	set<int>::iterator itr = mAckSids.find(death_sid);
	if(itr != mAckSids.end())	mAckSids.erase(itr);	
	
	mDeathSids.insert(death_sid);
}


bool
AosWaitInfo::isAckFinish()
{
	// death svr is another ack.
	return (mAckSids.size() + mDeathSids.size()) >= mNeedAckNum; 
}


bool 	
AosWaitInfo::isAllSvrDeath()
{
	// Ketty 2013/09/16
	return mDeathSids.size() == mTrans->getBkpSvrNum();
}


/*
bool
AosWaitInfo::canWakeup()
{
	if(isAckFinish())	return true;

	u32 wakeup_ack_num = eWakeupAckNum;
	if(wakeup_ack_num > mNeedAckNum)	wakeup_ack_num = mNeedAckNum;

	return mAckSids.size() >= wakeup_ack_num;
}
*/

OmnString
AosWaitInfo::toString()
{
	OmnString ss;
	ss << "; trans_id:" << mTrans->getTransId().toString()
		<< "; type:" << mTrans->getStrType()
		<< "; is_need_resp:" << mTrans->isNeedResp()
		<< "; need_ack_num:" << mNeedAckNum
		<< "; recv_ack:";
	set<int>::iterator itr;
	for(itr = mAckSids.begin(); itr != mAckSids.end(); itr++)
	{
		ss << *itr << "_";
	}
	
	ss << "; death_sid:";
	for(itr = mDeathSids.begin(); itr != mDeathSids.end(); itr++)
	{
		ss << *itr << "_";
	}
	return ss;
}
