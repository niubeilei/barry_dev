////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_SeRequestProc_h
#define Omn_ReqProc_SeRequestProc_h

#include "ReqProc/ReqProc.h"
#include "SeReqProc/Ptrs.h"
#include "SeReqProc/Reqids.h"
#include "SEServer/Ptrs.h"
#include "Util/String.h"


class AosSeRequestProc : public AosReqProc
{
	OmnDefineRCObject;

private:
	AosSeReqid::E	mId;
	OmnString		mReqidName;

protected:
	AosSeReqProcPtr	mSeReqProc;

public:
	AosSeRequestProc(
			const OmnString &reqid, 
			const AosSeReqid::E id, 
			const bool regflag);
	~AosSeRequestProc();

	// SeRequestProc Interface
	virtual bool proc(const AosRundataPtr &rdata) = 0;

	// Chen Ding, 2013/03/06
	virtual AosSeRequestProcPtr clone(const AosRundataPtr &rdata);
	virtual bool requestFinished();
	virtual bool requestFailed();
	
	void setSeReqProc(const AosSeReqProcPtr &p);// {mSeReqProc = p;}
	OmnString getReqidName() const {return mReqidName;}
	static AosSeRequestProcPtr 	getProc(const OmnString &reqid);

private:
	bool 	registerSeProc(const AosSeRequestProcPtr &proc, OmnString &errmsg);
};
#endif

