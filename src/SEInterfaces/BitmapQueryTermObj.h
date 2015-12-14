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
// 2013/02/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapQueryTermObj_h
#define Aos_SEInterfaces_BitmapQueryTermObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"


class AosBitmapQueryTermObj : public AosJimo
{
protected:
	static AosBitmapQueryTermObjPtr 	smObject;

public:
	AosBitmapQueryTermObj(const int version);

	virtual u64 	getMatchedSize()const{return 0;}
	virtual i64 	getBlockStart()const{return 0;}
	virtual i64 	getQueryCursor()const{return 0;}
	virtual u64 	getTotalInRslt()const{return 0;}
	virtual bool 	finished()const{return true;}
	virtual bool 	runQuery(const AosRundataPtr &rdata) = 0;
	virtual void 	clear() = 0;
	virtual bool 	isSuccess() const {return false;}
	virtual OmnString getErrmsg() const {return "";}
	virtual AosQueryReqObjPtr getQueryReq() const = 0;

	virtual bool andTermFinished(const AosRundataPtr &rdata, 
						const AosBitmapQueryTermObjPtr &and_term) = 0;
	virtual bool andTermFailed(const AosRundataPtr &rdata, 
						const AosBitmapQueryTermObjPtr &and_term,
						const OmnString &errmsg) = 0;
	virtual bool	reset() = 0;
	virtual bool	singleMoveTo(const u64 &startidx,
	               		const AosRundataPtr &rdata) = 0;

	virtual bool	moveTo(const u64 &startidx,
	               		const AosRundataPtr &rdata) = 0;
	
	virtual void setCaller(const AosQueryCallerPtr &caller) = 0;
	virtual void setQueryReq(const AosQueryReqObjPtr &req) = 0;

	virtual AosBitmapQueryTermObjPtr createTerm(
						const AosRundataPtr &rdata,
						const AosQueryCallerPtr &query_req, 
						const AosQueryTermObjPtr &term,
						const AosQueryReqObjPtr &req, 
						const bool check_paral) = 0;

	// Chen Ding, 2014/04/05
	virtual OmnString getBitmapQueryTermType() const = 0;
	virtual OmnString getBitmapQueryTermInfo() const = 0;
	virtual u64		  getBitmapQueryTermId() const = 0;
	virtual bool	setEstimateFlag(const bool flag){return true;}	

	// Chen Ding, 2014/04/14
	static AosBitmapQueryTermObjPtr createTermStatic(
						const AosRundataPtr &rdata,
						const AosQueryCallerPtr &query_req, 
						const AosQueryTermObjPtr &term,
						const AosQueryReqObjPtr &req, 
						const bool check_paral);

private:
	static AosBitmapQueryTermObjPtr getBitmapQueryTermObj(const AosRundataPtr &rdata);
};
#endif

