///////////////////////////////////////////////////////////////////////////
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
// 03/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_CommitSnapShot_h
#define AOS_IILTrans_CommitSnapShot_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Ptrs.h"


class AosIILTransCommitSnapShot : public AosIILTrans
{
	u64		 	mTaskDocid;
	u32		 	mVirtualId;
public:
	AosIILTransCommitSnapShot(const bool regflag);
	AosIILTransCommitSnapShot(
			const u64 &taskdocid,
			const u32 &virtual_id,
			const u64 &snap_id,
			const bool need_save,
			const bool need_resp);
	~AosIILTransCommitSnapShot(){}

	// IILTrans Interface
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	// IILTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata){return true;};
	//virtual bool needProc() const {return true;}
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;
};

#endif
