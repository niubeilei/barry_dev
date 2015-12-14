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
// 2014/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallComm_h
#define Aos_JimoCall_JimoCallComm_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DataTypes.h"
#include "Util/ValueRslt.h"
#include "UtilComm/UdpComm.h"

class JimoCallComm : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnUdpCommPtr		mComm;

public:
	JimoCallComm();
};
#endif

