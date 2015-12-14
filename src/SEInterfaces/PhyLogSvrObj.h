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
// 2013/03/11 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_PhyLogSvrObj_h
#define Aos_SEInterfaces_PhyLogSvrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"


class AosPhyLogSvrObj : virtual public OmnRCObject
{
protected:
	static AosPhyLogSvrObjPtr		smPhyLogSvr;

public:
	static void setPhyLogSvr(const AosPhyLogSvrObjPtr &logsvr) {smPhyLogSvr = logsvr;}
	static AosPhyLogSvrObjPtr getPhyLogSvr() {return smPhyLogSvr;}

protected:
};

#endif

