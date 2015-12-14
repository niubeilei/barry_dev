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
// Modification History:
// 2014/04/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryEngine_BitmapQueryDummy_h
#define Aos_QueryEngine_BitmapQueryDummy_h

#include "QueryEngine/Ptrs.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/BitmapQueryTermObj.h"
#include "SEInterfaces/QueryProcObj.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include <vector>
using namespace std;


class AosBitmapQueryDummy : virtual public AosBitmapQueryTermObj
{
	OmnDefineRCObject;

private:
	
public:
	AosBitmapQueryDummy(const int ver);
	~AosBitmapQueryDummy();

	virtual AosJimoPtr cloneJimo()const {return 0;}
	virtual bool andTermFinished(const AosRundataPtr &rdata, 
						const AosBitmapQueryTermObjPtr &and_term);
	virtual bool andTermFailed(const AosRundataPtr &rdata, 
						const AosBitmapQueryTermObjPtr &and_term,
						const OmnString &errmsg);
	virtual AosQueryReqObjPtr getQueryReq() const;
	virtual bool 		runQuery(const AosRundataPtr &rdata);
	virtual void 		clear();
	virtual bool		reset();
	virtual bool		moveTo(const u64 &startidx, const AosRundataPtr &rdata);
	virtual OmnString 	getBitmapQueryTermType() const;
	virtual OmnString 	getBitmapQueryTermInfo() const;
	virtual u64		  	getBitmapQueryTermId() const;
};
#endif
