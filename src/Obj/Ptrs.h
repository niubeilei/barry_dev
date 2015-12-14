////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//	This is the database for MySQL.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SysObj_Ptrs_h
#define Omn_SysObj_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(OmnSysObj, OmnSysObjPtr);
OmnPtrDecl(OmnObj, OmnObjPtr);
OmnPtrDecl(OmnSeqKeyedObj, OmnSeqKeyedObjPtr);
OmnPtrDecl(OmnMngdObj, OmnMngdObjPtr);
OmnPtrDecl(OmnMngdObjMgr, OmnMngdObjMgrPtr);
OmnPtrDecl(OmnPcObj, OmnPcObjPtr);
OmnPtrDecl(OmnDbObj, OmnDbObjPtr);
OmnPtrDecl(OmnMgdDbObj, OmnMgdDbObjPtr);
OmnPtrDecl(OmnPcObjMgr, OmnPcObjMgrPtr);
OmnPtrDecl(OmnNotInDbObj, OmnNotInDbObjPtr);

#endif
