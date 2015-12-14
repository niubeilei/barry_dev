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
// 2015/03/16	Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataSyncObj_h
#define Aos_SEInterfaces_DataSyncObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosDataSyncObj : virtual public OmnRCObject
{
	static AosDataSyncObjPtr	smDataSync;

public:
	static void setDataSync(const AosDataSyncObjPtr &d) {smDataSync = d;}
	static AosDataSyncObjPtr getDataSync() {return smDataSync;}
	
	virtual bool init(const AosRundataPtr &rdata) = 0;
	virtual bool start(const AosRundataPtr &rdata) = 0;
	virtual bool stop(const AosRundataPtr &rdata) = 0;

};

#endif
