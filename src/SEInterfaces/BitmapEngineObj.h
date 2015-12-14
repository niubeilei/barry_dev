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
#ifndef Aos_SEInterfaces_BitmapEngineObj_h
#define Aos_SEInterfaces_BitmapEngineObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosBitmapEngineObj : public AosJimo
{
	static AosBitmapEngineObjPtr	smObject;

public:
	AosBitmapEngineObj(const int version);

	static AosBitmapEngineObjPtr getBitmapEngine();

	virtual AosIILExecutorObjPtr getIILExecutor(const AosRundataPtr &rdata)= 0;
	virtual AosIILExecutorObjPtr getIILExecutorByID(const u64 &id,  const AosRundataPtr &rdata)= 0;
	virtual void IILExecutorFinished(const u64 &executor_id) = 0;
	virtual bool addCaller(
				const AosRundataPtr &rdata, 
				const u64 &bitmap_id, 
				const AosBmpExeCallerPtr &caller) = 0;

	//static int getPhysicalId(const u64 &section_id);
	static int getCubeId(const u64 &section_id);
};

inline AosBitmapEngineObjPtr AosGetBitmapEngine() 
{
 	return AosBitmapEngineObj::getBitmapEngine();
}
#endif

