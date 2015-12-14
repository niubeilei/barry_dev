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
// 07/17/2013 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_TransId_h
#define Aos_TransUtil_TransId_h

#include "aosUtil/Types.h"
#include "Util/Buff.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

struct AosTransId
{
	enum
	{
		eEntrySize = sizeof(u16) + sizeof(u16) + sizeof(u64) 
	};

	u16     mFromSid;
	u16     mFromPid;
	u64     mSeqno;

	AosTransId()
	{
		mFromSid = 0;
		mFromPid = 0;
		mSeqno = 0;
	}

	static AosTransId Invalid;
	
	bool operator != (const AosTransId &lhs) const
	{
		return !(operator==(lhs));
	}
	
	bool operator == (const AosTransId &lhs) const
	{
		return mFromSid == lhs.mFromSid && mFromPid == lhs.mFromPid && mSeqno == lhs.mSeqno;
	}
	
	bool operator < (const AosTransId &lhs) const
	{
		//return memcmp(this, &lhs, sizeof(AosTransId));
		if(mFromSid != lhs.mFromSid) return mFromSid < lhs.mFromSid;	
		if(mFromPid != lhs.mFromPid) return mFromPid < lhs.mFromPid;	
		
		return mSeqno < lhs.mSeqno;
	}

	static AosTransId serializeFrom(const AosBuffPtr &buff)
	{
		u16 from_sid = buff->getU16(0);
		u16 from_pid = buff->getU16(0);
		u64 seqno = buff->getU64(0);
		
		//AosTransId trans_id = {from_sid, from_pid, seqno};
		AosTransId trans_id;
		trans_id.mFromSid = from_sid;
		trans_id.mFromPid = from_pid;
		trans_id.mSeqno = seqno;
		return trans_id;
	}

	bool serializeTo(const AosBuffPtr &buff)
	{
		aos_assert_r(buff, false);
		buff->setU16(mFromSid);
		buff->setU16(mFromPid);
		buff->setU64(mSeqno);
		return true;
	}

	OmnString toString() const
	{
		OmnString id_str;
		id_str << mFromSid << "; " << mFromPid << "; " << mSeqno << "; ";
	
		return id_str;
	}

	u64	getCltKey() const
	{
		u64 key = mFromSid;
		key = (key << 32) + mFromPid;
		return key;
	}

	u64 getSeqno() const { return mSeqno; };

	static u64 size(){return eEntrySize;}
};

#endif
