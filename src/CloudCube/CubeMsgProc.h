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
// Modification History:
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CloudCube_CubeMsgProc_h
#define Aos_CloudCube_CubeMsgProc_h

#include "CloudCube/Ptrs.h"
#include "Message/Ptrs.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosCubeMsgProc : virtual public OmnRCObject
{
protected:

public:
	virtual bool procMsg(
					const AosRundataPtr &rdata,
					const AosBuffMsgPtr &msg) = 0;
	virtual AosCubeMsgProcPtr cloneCubeMsgProc() = 0;
};
#endif

