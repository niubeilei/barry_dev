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
// Created: 08/09/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_TargetInfoUtil_h
#define AOS_SvrProxyMgr_TargetInfoUtil_h

#include "TransUtil/Ptrs.h"
#include "TransBasic/Trans.h"
#include "Util/TransId.h"

struct AosTransIdCompFun
{
	bool operator() (const AosTransPtr &lhs, const AosTransPtr &rhs)
	{
		return lhs->getTransId() < rhs->getTransId();
	}
};

typedef set<AosTransPtr, AosTransIdCompFun> set_f;
typedef set<AosTransPtr, AosTransIdCompFun>::iterator setitr_f;
typedef set<AosTransPtr, AosTransIdCompFun>::reverse_iterator setritr_f;


struct TransIdExpand
{
	AosTransId mTransId;
	u32		   mToSvrId;
	u32		   mToProcId;
	
	bool operator < (const TransIdExpand &lhs) const
	{
		if(mTransId != lhs.mTransId)	return mTransId < lhs.mTransId;
		if(mToSvrId != lhs.mToSvrId)	return mToSvrId < lhs.mToSvrId;
		return mToProcId < lhs.mToProcId;
	}

};

#endif
