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
// 07/19/2011	: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Proggie_ReqDistr_ConnListener_h
#define Aos_Proggie_ReqDistr_ConnListener_h

#include "Proggie/ProggieUtil/Ptrs.h"
#include "Proggie/ReqDistr/Ptrs.h"
#include "Util/RCObject.h"
#include "UtilComm/Ptrs.h"
#include "Util/String.h"
#include <vector>


class AosConnListener : virtual public OmnRCObject
{
public:
	virtual void				connCreated(const OmnTcpClientPtr &conn) {return;}
	virtual void				connClosed(const OmnTcpClientPtr &conn) {return;}
};
#endif

