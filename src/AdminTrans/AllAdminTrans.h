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
// 03/25/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AdminTrans_AllAdminTrans_h
#define Aos_AdminTrans_AllAdminTrans_h

#include "AdminTrans/AdminStartProcessTrans.h"
#include "AdminTrans/AdminStopProcessTrans.h"
#include "AdminTrans/AdminAddServerTrans.h"
#include "AdminTrans/AdminRunCmdTrans.h"

class AosAllAdminTrans
{
public:
	static void init();
};

#endif
