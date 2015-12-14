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
//   
//
// Modification History:
// 05/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_NetListener_h
#define Aos_SEInterfaces_NetListener_h

#include "ErrorMgr/ErrorCode.h"
#include "UtilData/ModuleId.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"

class AosNetListener : virtual public OmnRCObject
{
public:
	virtual bool commandReceived(
						const int virtual_id, 
						const AosModuleId::E module_id, 
						const AosNetCommand::E command,
						const AosBuffPtr &buff, 
						AosErrorCode::E &errcode, 
						bool &sendResp) = 0;
};
#endif

