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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ThreadShellRunners_DistrBlobCaller_h
#define Aos_ThreadShellRunners_DistrBlobCaller_h

#include "DataBlob/Ptrs.h"
#include "DataSource/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObjImp.h"

class AosDistrBlobCaller : virtual public OmnRCObject
{

public:
	virtual OmnFilePtr getFile(const u32 file_idx, const AosRundataPtr &rdata) = 0;
	virtual bool threadProcFinished(const AosDistrBlobToBucketRunnerPtr &runner) = 0;
};
#endif

