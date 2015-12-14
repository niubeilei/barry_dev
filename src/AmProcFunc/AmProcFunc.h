////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2007-04-17: Created by CHK
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmProcFunc_AmProcFunc_h
#define Aos_AmProcFunc_AmProcFunc_h
 
#include "AccessManager/Ptrs.h"
#include "AmUtil/Ptrs.h"
#include "AmUtil/AmRequestTypeId.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "Util/HashSPtr.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"


typedef int (*AosAmProcFunc)(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);


int AmAttLocal(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

int AmAttExternal 	(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

int AmAttLdap 		(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

int AmAttRadius 		(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

int AmAttActivedirectory(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

int AmAttReserved 	(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

#endif // Aos_AmProcFunc_AmProcFunc_h

