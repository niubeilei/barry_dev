
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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/StatTrans.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TransUtil/TransProcThrd.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlDoc.h"

AosTransProcThrdPtr 	AosStatTrans::smProcThrd = OmnNew AosTransProcThrd("statTrans", 4);

AosStatTrans::AosStatTrans(
		const AosTransType::E type, 
		const bool reg_flag)
:
AosCubicTrans(type, reg_flag)
{
}


AosStatTrans::AosStatTrans(
		const AosTransType::E type,
		const u32 cube_id,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(type, cube_id, true, need_save, need_resp)
{
}


AosStatTrans::~AosStatTrans()
{
}


bool
AosStatTrans::serializeTo(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeTo(buff);
	return true;
}


bool
AosStatTrans::serializeFrom(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeFrom(buff);
	return true;
}


bool	
AosStatTrans::directProc()
{
	//u32 idx = mDistId % smProcThrd->getNumThrds();
	u32 idx = rand() % smProcThrd->getNumThrds();
	AosTransPtr thisptr(this, false);
	return smProcThrd->addRequest(idx, thisptr);
}

