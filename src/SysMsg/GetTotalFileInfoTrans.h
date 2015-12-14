// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysMsg_GetTotalFileInfoTrans_h
#define Aos_SysMsg_GetTotalFileInfoTrans_h

#include "TransUtil/TaskTrans.h"

class AosGetTotalFileInfoTrans : virtual public AosTaskTrans
{

private:
	u32			mCubeId;

public:
	AosGetTotalFileInfoTrans(const bool regflag);
	AosGetTotalFileInfoTrans(
			const int svr_id,
			const u32 cube_grp_id,
			const u32 cube_id);
	~AosGetTotalFileInfoTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eGetTotalFileInfo"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool proc();
	
private:

};
#endif

