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
#ifndef AOS_TransBasic_AppMsg_h
#define AOS_TransBasic_AppMsg_h

#include "SEInterfaces/ProcessType.h"
#include "TransBasic/ConnMsg.h"
#include "Util/Buff.h"

class AosAppMsg: public AosConnMsg
{

public:
	enum
	{
		eDataLen = 29,
		eIsTransOff = 28,

		eAppMsgLen = AosConnMsg::eConnMsgLen + eDataLen,
	};

private:
	int		mFromSvrId;
	u32		mFromProcId;
	int		mToSvrIdOff;
	int		mToSvrId;
	u32		mToProcId;
	AosProcessType::E	mToProcType;
	int		mCubeGrpId;

	int		mRlbSeqOff;
	u32		mRlbSeq;
	
public:
	AosAppMsg(
		const MsgType::E tp,
		const bool reg_flag);
	AosAppMsg(
		const MsgType::E tp,
		const int to_svrid,
		const u32 to_proc_id);
	AosAppMsg(
		const MsgType::E tp,
		const int to_svrid,
		const AosProcessType::E to_proc_tp,
		const int cube_grp_id);
	virtual ~AosAppMsg();
	
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual bool	proc();
	virtual bool	directProc();
	virtual bool 	isAppMsg(){ return true;};
	virtual bool 	isTrans(){ return false;};
	
	static AosConnMsgPtr serializeStatic(const AosBuffPtr &buff, 
							const u32 beg_idx,
							const MsgType::E tp);
	
	int		getFromSvrId(){ return mFromSvrId; };	
	u32		getFromProcId(){ return mFromProcId; };	
	int		getToSvrId() {return mToSvrId;};
	u32		getToProcId() {return mToProcId;};
	AosProcessType::E	getToProcType() {return mToProcType;};
	int		getCubeGrpId() {return mCubeGrpId;};
	
	bool	isToSvrProxy(){ return mToProcId == 0 && mToProcType == AosProcessType::eInvalid; };

	void	setToSvrId(const int svr_id);
	void	setToProcId(const u32 proc_id);
	void 	setRlbSeq(const u32 rlb_seq);

	u32  	getRlbSeq(){ return mRlbSeq; };

};

#endif
