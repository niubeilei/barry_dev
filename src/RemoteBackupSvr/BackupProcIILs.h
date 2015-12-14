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
#ifndef AOS_RemoteBackupSvr_ProcIILs_h
#define AOS_RemoteBackupSvr_ProcIILs_h

#include "RemoteBackupSvr/BackupProc.h"


class AosBackupProcIILs : public AosBackupProc
{
public:
	AosBackupProcIILs(const bool flag);
	~AosBackupProcIILs() {}

	virtual bool proc(	const u64 &transid, 
						const AosXmlTagPtr &doc,
						const AosRundataPtr &rdata);
};
#endif

