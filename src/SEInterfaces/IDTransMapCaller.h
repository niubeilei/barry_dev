////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/01/21	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_IDTransMapCaller_h
#define AOS_SEInterfaces_IDTransMapCaller_h 

#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IDTransMapObj.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosIDTransMapCaller : virtual public OmnRCObject
{
public:
	virtual bool	procTrans(
						const u64 &id,
						const u32 siteid,
						const AosIDTransVectorPtr &p,
						const AosRundataPtr &rdata) = 0;
	virtual bool	procOneTrans(
						const u64 &id,
						const u32 siteid,
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata) = 0;
	virtual bool	checkNeedProc(
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata) = 0;
};

#endif
