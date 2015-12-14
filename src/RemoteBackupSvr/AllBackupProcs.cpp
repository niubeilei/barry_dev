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
// 01/23/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupSvr/AllBackupProcs.h"

#include "RemoteBackupSvr/BackupProcDocCreated.h"
#include "RemoteBackupSvr/BackupProcDocModified.h"
#include "RemoteBackupSvr/BackupProcDocDeleted.h"
#include "RemoteBackupSvr/BackupProcIILs.h"
#include "RemoteBackupSvr/BackupProcAddWord.h"
#include "Util/HashUtil.h"

AosBackupProcPtr		sgProcs[AosRemoteBkType::eMax];
AosStr2U32_t         		sgMap;

AosAllBackupProcs gAosAllBackupProcs;

AosAllBackupProcs::AosAllBackupProcs()
{
	static AosBackupProcDocCreated          sgDocCreated(true); 
	static AosBackupProcDocModified         sgDocModified(true);
	static AosBackupProcDocDeleted          sgDocDeleted(true); 
	static AosBackupProcIILs                sgIILs(true); 
	static AosBackupProcAddWord				sgAddWord(true);
}



