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
#include "SysTrans/AllSysTrans.h"

#include "SysTrans/ThreadMonitorTrans.h"
#include "SysTrans/AskWheatherDeathTrans.h"
#include "SysTrans/TellServerDeathTrans.h"

//AosAllSysTrans gAosAllSysTrans;

//AosAllSysTrans::AosAllSysTrans()
void
AosAllSysTrans::init()
{

static AosThreadMonitorTrans			sgThreadMonitorTrans(true);
static AosAskWheatherDeathTrans			sgAskWheatherDeathTrans(true);
static AosTellServerDeathTrans			sgTellServerDeathTrans(true);

}

