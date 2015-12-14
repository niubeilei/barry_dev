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
#include "AdminTrans/AllAdminTrans.h"

void
AosAllAdminTrans::init()
{
	static AosAdminStartProcessTrans			sgAdminStartProcessTrans(true);
	static AosAdminStopProcessTrans				sgAdminStopProcessTrans(true);
	static AosAdminAddServerTrans				sgAdminAddServerTrans(true);
	static AosAdminRunCmdTrans					sgAdminRunCmdTrans(true);
}

