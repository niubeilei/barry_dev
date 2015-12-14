////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgrListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgr_AppMgrListener_h
#define Omn_AppMgr_AppMgrListener_h

#include "Util/RCObject.h"


class OmnAppMgrListener : virtual public OmnRCObject
{
public:
	virtual void	procAppMgrEvent(const OmnEventPtr &event) = 0;
};

#endif
