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
// 2013/05/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_BackupLog_BackupLog_h
#define AOS_BackupLog_BackupLog_h

#include "SEInterfaces/BackupLogObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosBackupLog : virtual public AosBackupLogObj
{
private:
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosBackupLog();
	virtual ~AosBackupLog();

	virtual bool appendLog(	const AosRundataPtr &rdata, 
							const u64 docid, 
							const AosBuffPtr &body);
	virtual bool checkSystemRestart(const AosRundataPtr &rdata);
	virtual bool recoverDoc(const AosRundataPtr &rdata, 
							const u64 docid,
							AosBuffPtr &body);

};
#endif

