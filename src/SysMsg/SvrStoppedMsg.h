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
#ifndef Aos_SysMsg_SvrStoppedMsg_h
#define Aos_SysMsg_SvrStoppedMsg_h

#include "TransBasic/AppMsg.h"

class AosSvrStoppedMsg : public AosAppMsg
{

private:
	int		mStoppedSid;

public:
	AosSvrStoppedMsg(const bool regflag);
	AosSvrStoppedMsg(
			const int to_svr_id,
			const int stoped_sid);
	~AosSvrStoppedMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eSvrStoppedMsg"; };
	virtual bool proc();
	
};
#endif

