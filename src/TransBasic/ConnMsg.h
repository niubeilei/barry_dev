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
#ifndef AOS_Transbasic_ConnMsg_h
#define  AOS_Transbasic_ConnMsg_h

#include "TransBasic/Ptrs.h"
#include "TransBasic/MsgType.h"
#include "Util/Buff.h"
	
class AosConnMsg: virtual public OmnRCObject 
{
	OmnDefineRCObject;
	
	protected:
	enum
	{
		eConnMsgLen = sizeof(u16) + sizeof(u8), 
	};

private:
	MsgType::E  mType;

protected:
	AosBuffPtr	mConnBuff;		// the buff. used to send and read.

public:
	AosConnMsg(const MsgType::E tp, const bool reg_flag);
	virtual ~AosConnMsg();

	static AosConnMsgPtr serializeFromStatic(const AosBuffPtr &buff, 
							const bool set_conn_buff);
	//static AosConnMsgPtr serializeStatic(const AosBuffPtr &buff);

	virtual bool	 serializeTo(const AosBuffPtr &buff) = 0;
	virtual bool	 serializeFrom(const AosBuffPtr &buff) = 0;
	virtual AosConnMsgPtr clone2() = 0;
	virtual bool	 isAppMsg(){ return false;};
	virtual AosConnMsgPtr copy();
	virtual OmnString getStrType();

	MsgType::E		getType(){ return mType; };
	AosBuffPtr		getConnBuff();
	u64				getSize();

	// For Trans resend. tmp.
	void			resetConnBuff(){ mConnBuff = 0; };

	static bool registerDynMsg(const MsgType::E tp, const AosConnMsgPtr &msg);

private:
	bool 	registerMsg(const MsgType::E tp, const AosConnMsgPtr &msg);
	
};
#endif
