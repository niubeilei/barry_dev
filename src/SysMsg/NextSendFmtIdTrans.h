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
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysMsg_NextSendFmtIdTrans_h
#define AOS_SysMsg_NextSendFmtIdTrans_h

#include "TransUtil/TaskTrans.h"
#include "Util/Buff.h"

class AosNextSendFmtIdTrans: public AosTaskTrans
{

private:
	u32		mCubeGrpId;

public:
	AosNextSendFmtIdTrans(const bool reg_flag);
	AosNextSendFmtIdTrans(
		const int to_svrid,
		const u32 cube_grp_id);
	~AosNextSendFmtIdTrans();
	
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eMasterNextSend"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool	proc();
	
};

#endif
