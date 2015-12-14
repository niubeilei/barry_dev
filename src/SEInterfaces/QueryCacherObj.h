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
// 2013/12/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryCacherObj_h
#define Aos_SEInterfaces_QueryCacherObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryProcObj.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "Util/RCObject.h"



class AosQueryCacherObj : virtual public AosQueryProcCallback, virtual public AosQueryProcObj
{
	OmnDefineRCObject;

private:
	static AosQueryCacherObjPtr		smCacher;

public:
	AosQueryCacherObj();

	virtual bool runQuery(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query_req, 
					const AosQueryProcCallbackPtr &callback) = 0;
	virtual AosQueryCacherObjPtr cloneCacher() const = 0;

	virtual void setPageSize(const u64 &size) = 0;
	static void setCacher(const AosQueryCacherObjPtr &cacher) {smCacher = cacher;}
	static AosQueryCacherObjPtr getCacher() {return smCacher;}
	static AosQueryCacherObjPtr createQueryCacherStatic(const AosRundataPtr &rdata);
};
#endif

