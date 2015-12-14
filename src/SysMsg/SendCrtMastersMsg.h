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
#ifndef Aos_SysMsg_SendCrtMastersMsg_h
#define Aos_SysMsg_SendCrtMastersMsg_h

#include "TransBasic/AppMsg.h"

class AosSendCrtMastersMsg : public AosAppMsg
{

private:	
	bool		mGetBuffSucc;
	bool		mFromFile;
	AosBuffPtr mMastersBuff;

public:
	AosSendCrtMastersMsg(const bool regflag);
	AosSendCrtMastersMsg(
			const int to_svr_id,
			const bool from_file,
			const AosBuffPtr &masters_buff);
	AosSendCrtMastersMsg(const int to_svr_id);
	~AosSendCrtMastersMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eSendCrtMastersMsg"; };
	virtual bool proc();
	
};
#endif

