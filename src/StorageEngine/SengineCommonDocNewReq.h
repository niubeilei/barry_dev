////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2015/05/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageEngine_SengineCommonDocNewReq_h
#define AOS_StorageEngine_SengineCommonDocNewReq_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/SengineDocReqObj.h"


class AosSengineCommonDocNewReq : public AosSengineDocReqObj 
{

public:
	AosBuffPtr	mBuff;

public:
	AosSengineCommonDocNewReq(
			const req_type type,
			const AosTransPtr &trans,
			const int vid,
			const u64 group_id,
			const AosBuffPtr &buff,
			const u64 &snap_id)
	:
	AosSengineDocReqObj(type, trans, vid, group_id, snap_id),
	mBuff(buff)
	{
	}


	~AosSengineCommonDocNewReq()
	{
	}
};

#endif

