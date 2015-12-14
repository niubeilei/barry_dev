////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 12/17/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_XmlInterface_Server_SyncServerReqCb_h
#define AOS_XmlInterface_Server_SyncServerReqCb_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/Types.h"


class AosSyncServerReqCb : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual bool 	syncServerCb(const AosSyncServerReqType, void *data) = 0;
};

#endif
