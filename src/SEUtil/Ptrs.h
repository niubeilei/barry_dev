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
// 	Created: 10/08/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_Ptrs_h
#define Omn_SEUtil_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosDocFileMgr, AosDocFileMgrPtr)
OmnPtrDecl(AosVersionDocMgr, AosVersionDocMgrPtr)
OmnPtrDecl(AosDiskBlock, AosDiskBlockPtr)
OmnPtrDecl(AosDiskBlockMgr, AosDiskBlockMgrPtr)
OmnPtrDecl(AosIdleBlockMgr, AosIdleBlockMgrPtr)
OmnPtrDecl(AosMonitorLogMgr, AosMonitorLogMgrPtr)
OmnPtrDecl(AosXmlGenerateRule, AosXmlGenerateRulePtr)
//OmnPtrDecl(AosSesionObj, AosSesionObjPtr)
OmnPtrDecl(AosLogFile, AosLogFilePtr)
OmnPtrDecl(AosProgressMonitor, AosProgressMonitorPtr)

#endif
