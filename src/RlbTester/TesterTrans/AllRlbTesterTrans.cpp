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
#include "RlbTester/TesterTrans/AllRlbTesterTrans.h"

#include "RlbTester/TesterTrans/RlbTesterCreateFileTrans.h"
#include "RlbTester/TesterTrans/RlbTesterModifyFileTrans.h"
#include "RlbTester/TesterTrans/RlbTesterDeleteFileTrans.h"
#include "RlbTester/TesterTrans/RlbTesterReadDataTrans.h"
//#include "RlbTester/TesterTrans/RlbTesterKillProcMsg.h"
#include "RlbTester/TesterTrans/RlbTesterReadDataTrans.h"
#include "RlbTester/TesterTrans/RlbTesterStartProcMsg.h"
#include "RlbTester/TesterTrans/RlbTesterDamageDiskTrans.h"
#include "RlbTester/TesterTrans/RlbTesterIsProcDeathedTrans.h"
#include "RlbTester/TesterTrans/RlbTesterSwitchMFinishTrans.h"

void
AosAllRlbTesterTrans::init()
{

static AosRlbTesterCreateFileTrans 		sgRlbTesterCreateFileTrans(true);
static AosRlbTesterModifyFileTrans 		sgRlbTesterModifyFileTrans(true);
static AosRlbTesterDeleteFileTrans 		sgRlbTesterDeleteFileTrans(true);
static AosRlbTesterReadDataTrans   		sgRlbTesterReadDataTrans(true);
//static AosRlbTesterKillProcMsg     		sgRlbTesterKillProcMsg(true);
static AosRlbTesterStartProcMsg    		sgRlbTesterStartProcMsg(true);
static AosRlbTesterDamageDiskTrans 		sgRlbTesterDamageDiskTrans(true);
static AosRlbTesterIsProcDeathedTrans 	sgRlbTesterIsProcDeathedTrans(true);
static AosRlbTesterSwitchMFinishTrans 	sgRlbTesterSwitchMFinishTrans(true);

}

