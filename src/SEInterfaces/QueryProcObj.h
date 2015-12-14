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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryProcObj_h
#define Aos_SEInterfaces_QueryProcObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryCaller.h"

class AosQueryProcObj : virtual public AosQueryCaller
{
private:
	static AosQueryProcObjPtr	smObject;

public:
	AosQueryProcObj();

	virtual u64 getQueryId() const = 0;
	virtual bool runQuery(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query, 
					const AosQueryProcCallbackPtr &callback) = 0;

	static void setObject(const AosQueryProcObjPtr &o) {smObject = o;}
	static AosQueryProcObjPtr getObject() {return smObject;}

	virtual u64 getMatchedSize()const = 0;
	virtual i64 getBlockStart()const = 0;
	virtual i64 getNumRawEntries()const = 0;
	// virtual i64 getQueryCursor()const = 0;
	virtual u64 getTotalInRslt()const = 0;
	virtual bool finished()const = 0;
	virtual bool moveTo(const AosRundataPtr &rdata, const u64 &startidx) = 0;
	virtual bool singleMoveTo(const AosRundataPtr &rdata, const u64 &startidx) = 0;
	virtual void setPageSize(const u64 &size) = 0;
	virtual void setBlockSize(const i64 &size) = 0;
	virtual bool reset() = 0;
	virtual void setEstimateFlag(const bool flag){}	
};

#endif
