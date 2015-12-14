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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocServer_AllDocSvrProcs_h
#define Aos_DocServer_AllDocSvrProcs_h

#include "DocServer/ProcCreateDoc.h"
#include "DocServer/ProcCreateDocSafe.h"
#include "DocServer/ProcCreateExcDoc.h"
#include "DocServer/ProcCheckDocLock.h"
#include "DocServer/ProcDocLock.h"
#include "DocServer/ProcDeleteObj.h"
#include "DocServer/ProcGetDoc.h"
#include "DocServer/ProcIsDocDeleted.h"
#include "DocServer/ProcModifyObj.h"
#include "DocServer/ProcSaveToFile.h"
#include "DocServer/ProcBatchFixed.h"
#include "DocServer/ProcBatchVariable.h"
#include "DocServer/ProcBatchDelDocs.h"
#include "DocServer/ProcBatchGetDocs.h"
#include "DocServer/ProcIncrementValue.h"
#include "DocServer/ProcBatchSendDocids.h"
#include "DocServer/ProcBatchSendStatus.h"
//#include "DocServer/ProcCreateBinaryDoc.h"
//#include "DocServer/ProcDeleteBinaryDoc.h"
//#include "DocServer/ProcRetrieveBinaryDoc.h"

// Ken Lee 2013/01/15
#include "DocServer/ProcBatchFixedStart.h"
#include "DocServer/ProcBatchFixedFinish.h"
#include "DocServer/ProcBatchFixedClean.h"

#endif

