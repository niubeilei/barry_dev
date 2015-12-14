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
// 11/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_GroupQuery_h
#define Aos_SEInterfaces_GroupQuery_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"


class AosGroupQueryObj : public OmnRCObject
{
	static AosGroupQueryObjPtr	smObject;
protected:

public:
	AosGroupQueryObj()
	{
	}

	virtual ~AosGroupQueryObj();

//	virtual AosGroupQueryObjPtr clone(const AosRundataPtr &rdata) = 0;
	virtual bool addCond(
				const OmnString &iilname,
				const AosOpr	&opr,
				const OmnString	&value1,
				const OmnString	&value2) = 0;
	
	virtual bool addCond(
				const OmnString &iilname,
				const AosQueryContextObjPtr &context) = 0;
	virtual bool setGroupFields(
				const OmnString &iilname_f1,
				const bool &f1_reverse,
				const OmnString &iilname_f2,
				const bool &f2_reverse) = 0;

	virtual bool setGroupFields(
				const OmnString &iilname_f2,
				const bool &f2_reverse) = 0;

	virtual void setPageSize(const u64 &page_size) = 0;

	virtual bool nextBlock(AosBitmapObjPtr &bitmap) = 0;
	
	static AosGroupQueryObjPtr createQueryStatic(const AosRundataPtr &rdata);
	static bool setObject(const AosGroupQueryObjPtr &obj);
	static AosGroupQueryObjPtr getObject();
	
	virtual AosGroupQueryObjPtr createQuery(const AosRundataPtr &rdata) const = 0;

};
#endif



