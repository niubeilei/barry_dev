////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: File.cpp
// Description:
//
//
// Modification History:
// 03/31/2009  Sharon Shen 
//////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogServer_Ptrs_h
#define AOS_LogServer_Ptrs_h

#include "UtilComm/Ptrs.h"


OmnPtrDecl(AosLogFile, AosLogFilePtr);
OmnPtrDecl(AosSFLog, AosSFLogPtr);
OmnPtrDecl(AosNetMgr, AosNetMgrPtr);
OmnPtrDecl(AosNetReq, AosNetReqPtr);
OmnPtrDecl(AosNetReqProc, AosNetReqProcPtr);
OmnPtrDecl(AosLog, AosLogPtr);
OmnPtrDecl(AosLogReq, AosLogReqPtr);
OmnPtrDecl(AosLogReqProc, AosLogReqProcPtr);
OmnPtrDecl(AosLogMgr, AosLogMgrPtr);
OmnPtrDecl(AosLogEntry, AosLogEntryPtr);

#endif
