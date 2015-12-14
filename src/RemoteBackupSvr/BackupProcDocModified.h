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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RemoteBackupSvr_ProcDocModified_h
#define AOS_RemoteBackupSvr_ProcDocModified_h

#include "RemoteBackupSvr/BackupProc.h"


class AosBackupProcDocModified : public AosBackupProc
{
public:
	AosBackupProcDocModified(const bool flag);
	~AosBackupProcDocModified() {}

	virtual bool proc(	const u64 &transid, 
						const AosXmlTagPtr &req,
						const AosRundataPtr &rdata);
};
#endif

