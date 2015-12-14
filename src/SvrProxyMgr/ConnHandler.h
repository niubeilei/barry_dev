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
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_ConnHandler_h
#define AOS_SvrProxyMgr_ConnHandler_h

#include "SvrProxyMgr/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosRecvEventHandler: public OmnRCObject
{

public:
	virtual bool	msgRecvEvent(const AosSockConnPtr &conn) = 0;

};

#endif
