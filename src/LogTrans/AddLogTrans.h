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
// 03/20/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogTrans_AddLogTrans_h
#define Aos_LogTrans_AddLogTrans_h

#include "LogTrans/LogTrans.h"
#include "SeLogUtil/LogOpr.h"


class AosAddLogTrans : virtual public AosLogTrans
{
	AosLogOpr::E	mOpr;
	u64				mCtnrDocid;
	OmnString		mPctrObjid;
	OmnString		mLogName;
	AosXmlTagPtr	mLogInfo;

public:
	AosAddLogTrans(const bool regflag);
	AosAddLogTrans(
		const AosRundataPtr &rdata,
		const AosLogOpr::E opr,
		const OmnString &pctr_objid,
		const OmnString &log_name,
		const AosXmlTagPtr &log_info,
		const bool need_save,
		const bool need_resp);
	~AosAddLogTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	
	virtual bool procLog();

private:
	u64 	initCtnrDocid(
				const AosRundataPtr &rdata,
				const OmnString &pctr_objid);

};
#endif

