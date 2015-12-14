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
#ifndef AOS_SysMsg_SyncUnfinishTrans_h
#define AOS_SysMsg_SyncUnfinishTrans_h

#include "TransUtil/TaskTrans.h"
#include "Util/Buff.h"

class AosSyncUnfinishTrans: public AosTaskTrans
{

private:
	int		mReadId;

public:
	AosSyncUnfinishTrans(const bool reg_flag);
	AosSyncUnfinishTrans(
		const int to_svrid,
		const u32 cube_grp_id,
		const int read_id);
	~AosSyncUnfinishTrans();
	
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eSyncUnfinishTrans"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool	proc();
	
};

#endif
