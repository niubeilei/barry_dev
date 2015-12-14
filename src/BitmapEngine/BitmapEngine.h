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
// 2013/02/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapEngine_BitmapEngine_h
#define Aos_BitmapEngine_BitmapEngine_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosBitmapEngine : virtual public AosBitmapEngineObj 
{
	OmnDefineRCObject;
public:
	static AosBitmapEngine *        getSelf();
	virtual AosIILExecutorObjPtr getIILExecutor(const AosRundataPtr &rdata);
	virtual AosIILExecutorObjPtr getIILExecutorByID(const u64 &id,  const AosRundataPtr &rdata);
	virtual void IILExecutorFinished(const u64 &executor_id){}

	static int getPhysicalId(const u64 &section_id){return 0;}
	static AosBitmapEngine* smBitmapEngine;
};
#endif

