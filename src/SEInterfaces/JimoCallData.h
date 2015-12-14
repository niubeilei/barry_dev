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
// 2015/02/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JimoCallData_h
#define Aos_SEInterfaces_JimoCallData_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"

class AosCallData;

class AosJimoCallData
{
public:
	int		call_id;
//	int		int_value;

public:
	AosJimoCallData();
	AosJimoCallData(const AosJimoCallerPtr &caller, const int call_id);
	void reset();
	void callFinished(AosRundata *rdata);
	void callFailed(AosRundata *rdata);
	OmnString getErrmsg() const;
	void setErrmsg(const OmnString &errmsg);
	void setSuccess();
	bool isSuccess() const;
	void signal();
	void setWait();
	void setFailed(const OmnString &errmsg);
	bool isFinished() const;
	void setNeedCallback(bool flag);
	void setU64Value(const u64 f);
	u64 getU64Value() const;
	AosXmlTagPtr getXmlDoc(AosRundata *rdata, const int idx) const;
};

#endif
