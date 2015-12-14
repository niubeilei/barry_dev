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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/CubeCommUdp.h"


AosCubeCommUdp::AosCubeCommUdp(const OmnCommListenerPtr &caller)
:
AosCubeComm("cubecomm_udp", caller),
mSendId(0)
{
	mComm = OmnNew OmnUdpComm(mEPInfo.mIpAddr, mEPInfo.mPort, mName);
	if (!mComm)
	{
		OmnThrowException("failed to create upd comm");
	}
}


AosCubeCommUdp::AosCubeCommUdp(
		const AosEndPointInfo epinfo, 
		const OmnCommListenerPtr &caller)
:
AosCubeComm("cubecomm_udp", caller),
mSendId(0),
mEPInfo(epinfo)
{
	mComm = OmnNew OmnUdpComm(mEPInfo.mIpAddr, mEPInfo.mPort, mName);
	if (!mComm)
	{
		OmnThrowException("failed to create upd comm");
	}
}


AosCubeCommUdp::~AosCubeCommUdp()
{
}


bool 	
AosCubeCommUdp::connect(AosRundata *rdata)
{
	aos_assert_r(mComm, false);

	OmnString err;
	bool rslt = mComm->reconnect(err);
	aos_assert_r(rslt, false);

	return true;
}


bool 	
AosCubeCommUdp::close()
{
	if (!mComm) return true;
	return mComm->closeConn();
}


bool 	
AosCubeCommUdp::startReading(
		AosRundata *rdata, 
		const OmnCommListenerPtr &caller)
{
	mCaller = caller;
	if (!mCaller)
	{
		AosLogError(rdata, false, AosErrmsgId::eInternalError) << enderr;
		return false;
	}

	OmnCommListenerPtr thisptr(this, false);
	mComm->startReading(thisptr);

	return true;
}


bool 	
AosCubeCommUdp::stopReading()
{
	if (!mComm) return true;
	return mComm->stopReading(mCaller);
}


bool 	
AosCubeCommUdp::sendTo(
		AosRundata *rdata, 
		const AosEndPointInfo &remote_epinfo,
		AosBuff *buff)
{
	// This function sends 'data' to 'remote_epid'. 
	aos_assert_r(mComm, false);

	u64 send_id = __sync_add_and_fetch(&mSendId, 1);

	i64 length = buff->dataLen();
	int time = length / eMaxSendBuffSize + 1;

	if (time == 1)
	{
		buff->gotoEnd();
		buff->setU64(send_id);
		buff->setInt(time);
		buff->setInt(0);
		
		OmnRslt rslt = mComm->sendTo(buff->data(), buff->dataLen(),
			remote_epinfo.mIpAddr, remote_epinfo.mPort);
		aos_assert_r(rslt, false);
		return true;	
	}

	i64 each_len = length / time + 1;
	//i64 each_len = eMaxSendBuffSize;
	char * data = buff->data();
	for (int i=0; i<time; i++)
	{
		char dd[eMaxSendBuffSize + 20];
		AosBuff bf(dd, eMaxSendBuffSize + 20, 0, false AosMemoryCheckerArgs);
		bf.setBuff(data, each_len);
		bf.setU64(send_id);
		bf.setInt(time);
		bf.setInt(i);

		data += each_len;
		length -= each_len;	
		if (i == time - 2)
		{
			each_len = length;
		}

		OmnRslt rslt = mComm->sendTo(dd, bf.dataLen(),
			remote_epinfo.mIpAddr, remote_epinfo.mPort);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool		
AosCubeCommUdp::msgRead(const OmnConnBuffPtr &connbuff)
{
	aos_assert_r(mCaller, false);

	mLockRaw->lock();
	mQueue.push(connbuff);
	mCondVar->signal();
	mLockRaw->unlock();
	return true;
}


bool
AosCubeCommUdp::proc(const OmnConnBuffPtr &connbuff)
{
	char * data = connbuff->getData();
	int length = connbuff->getDataLength();

	u64 send_id = *(u64*)(data + length - sizeof(u64) - sizeof(int) - sizeof(int));
	int num = *(int*)(data + length - sizeof(int) - sizeof(int));
	int idx = *(int*)(data + length - sizeof(int));

	OmnString key;
	key << connbuff->getRemoteAddr().toString()
		<< "_" << connbuff->getRemotePort()
		<< "_" << send_id;

	bool rslt = true;
	map<OmnString, MsgReceived>::iterator itr = mMsgReceived.find(key);
	if (itr == mMsgReceived.end())
	{
		if (num == 1)
		{
			rslt = rebuildConnBuff(connbuff);
			aos_assert_r(rslt, false);
		}
		else
		{
			MsgReceived mr;
			mr.num = num;
			mr.connbuffs[idx] = connbuff;
			mMsgReceived[key] = mr; 
		}
	}
	else
	{
		aos_assert_r(num == itr->second.num, false);

		itr->second.connbuffs[idx] = connbuff;

		if (itr->second.connbuffs.size() == (size_t)num)
		{
			rslt = rebuildConnBuff(itr->second.connbuffs);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


bool
AosCubeCommUdp::rebuildConnBuff(const OmnConnBuffPtr &connbuff)
{
	uint length = connbuff->getDataLength();
	length -= (sizeof(u64)+sizeof(int)+sizeof(int));
	connbuff->setDataLength(length);
	return mCaller->msgRead(connbuff);
}


bool
AosCubeCommUdp::rebuildConnBuff(map<int, OmnConnBuffPtr> &connbuffs)
{
	OmnConnBuffPtr newbuff = OmnNew OmnConnBuff(connbuffs[0]);
	newbuff->setDataLength((uint)0);

	map<int, OmnConnBuffPtr>::iterator itr = connbuffs.begin();
	while (itr != connbuffs.end())
	{
		char * data = itr->second->getData();
		int length = itr->second->getDataLength();
		length -= (sizeof(u64)+sizeof(int)+sizeof(int));

		newbuff->append(data, length);
		itr++;
	}
	return mCaller->msgRead(newbuff);
}

