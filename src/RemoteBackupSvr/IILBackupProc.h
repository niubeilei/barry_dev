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
// 02/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RemoteBackupSvr_IILBackupProc_h
#define Aos_RemoteBackupSvr_IILBackupProc_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransClient/TransCltProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosIILBackupProc : public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnMutexPtr			mLock;
	AosTransClientPtr   mTransClient;

public:
	AosIILBackupProc();
	~AosIILBackupProc();

	bool config(const AosXmlTagPtr &config);
	bool addReq(const AosRundataPtr &rdata,
				const OmnString &request,
				const u64 &iilid);
};
#endif

