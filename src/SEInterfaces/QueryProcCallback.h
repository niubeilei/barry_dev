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
// 2013/08/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryProcCallback_h
#define Aos_SEInterfaces_QueryProcCallback_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosQueryProcCallback : virtual public OmnRCObject
{
public:
	virtual bool queryFinished(
					const AosRundataPtr &rdata,
					const AosQueryRsltObjPtr &results,
					const AosBitmapObjPtr &bitmap,
					const AosQueryProcObjPtr &proc) = 0;
	virtual bool queryFailed(
					const AosRundataPtr &rdata,
					const OmnString &errmsg) = 0;
};
#endif
