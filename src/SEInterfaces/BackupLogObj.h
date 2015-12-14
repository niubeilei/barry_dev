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
// 2013/05/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BackupLogObj_h
#define Aos_SEInterfaces_BackupLogObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosBackupLogObj : virtual public OmnRCObject
{
private:

public:
	virtual bool appendLog(	const AosRundataPtr &rdata, 
							const u64 docid, 
							const AosBuffPtr &body) = 0;
	virtual bool checkSystemRestart(const AosRundataPtr &rdata) = 0;
	virtual bool recoverDoc(const AosRundataPtr &rdata, 
							const u64 docid,
							AosBuffPtr &body) = 0;
};
#endif
