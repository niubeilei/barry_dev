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
#include "SysMsg/AllSysMsg.h"

#include "SysMsg/NextSendFmtIdTrans.h"
#include "SysMsg/ReadFmtTrans.h"
//#include "SysMsg/FinishFmtsTrans.h"
#include "SysMsg/GetMaxRecvedFmtIdTrans.h"
#include "SysMsg/SendFinishedFmtIdTrans.h"

#include "SysMsg/RebootProcMsg.h"
#include "SysMsg/SvrStoppedMsg.h"
#include "SysMsg/SendFmtTrans.h"
#include "SysMsg/SetNewMasterMsg.h"
#include "SysMsg/TriggerResendMsg.h"
#include "SysMsg/ResendEndMsg.h"
//#include "SysMsg/SwitchToBkpTrans.h"
#include "SysMsg/StopSendFmtTrans.h"
#include "SysMsg/SwitchToMasterMsg.h"
#include "SysMsg/SwitchToMasterFinishMsg.h"
#include "SysMsg/ReSwitchToMasterMsg.h"
#include "SysMsg/SyncUnfinishTrans.h"
#include "SysMsg/StartProcessMsg.h"
#include "SysMsg/AddClusterMsg.h"
#include "SysMsg/StopProcessMsg.h"
#include "SysMsg/AddServerMsg.h"
#include "SysMsg/SetProcStatusMsg.h"
#include "SysMsg/GetTotalFileInfoTrans.h"
#include "SysMsg/RecoverFileTrans.h"
#include "SysMsg/DiskBadMsg.h"
#include "SysMsg/SvrUpMsg.h"
#include "SysMsg/SvrDeathMsg.h"
#include "SysMsg/ProcUpMsg.h"
#include "SysMsg/ProcDeathMsg.h"

#include "SysMsg/StartDynamicProcMsg.h"
#include "SysMsg/StartProcFinishMsg.h"
#include "SysMsg/KillProcMsg.h"
#include "SvrProxyUtil/ConnMsgAck.h"
#include "SvrProxyUtil/ConnMsgGetWaitSeq.h"
#include "SvrProxyUtil/ConnMsgWaitSeq.h"

#include "TransUtil/TransAckMsg.h"
#include "TransUtil/TransRespTrans.h"
#include "TransUtil/TransSvrDeathMsg.h"
#include "SysMsg/GetCrtMastersMsg.h"
#include "SysMsg/SendCrtMastersMsg.h"
#include "SysMsg/GetClusterConfigMsg.h"
#include "SysMsg/StartJobMgrMsg.h"
#include "SysMsg/CheckStartJobSvrMsg.h"
#include "SysMsg/NotifyProcIsUp.h"

#include "SysMsg/TestTrans.h"

//AosAllSysMsg gAosAllSysMsg;

//AosAllSysMsg::AosAllSysMsg()
void
AosAllSysMsg::init()
{

static AosNextSendFmtIdTrans			sgAosNextSendFmtIdTrans(true);
static AosReadFmtTrans					sgAosReadFmtTrans(true);
//static AosFinishFmtsTrans				sgAosFinishFmtsTrans(true);
static AosGetMaxRecvedFmtIdTrans		sgAosGetMaxRecvedFmtIdTrans(true);
static AosSendFinishedFmtIdTrans		sgAosSendFinishedFmtIdTrans(true);

static AosRebootProcMsg					sgAosRebootProcMsg(true);
static AosSvrStoppedMsg					sgAosSvrStoppedMsg(true);
static AosSendFmtTrans					sgAosSendFmtTrans(true);
static AosSetNewMasterMsg				sgAosSetNewMasterMsg(true);
static AosTriggerResendMsg				sgTriggerResendMsg(true);
static AosResendEndMsg					sgResendEndMsg(true);
//static AosSwitchToBkpTrans				sgAosSwitchToBkpTrans(true);
static AosStopSendFmtTrans				sgAosStopSendFmtTrans(true);
static AosSwitchToMasterMsg				sgAosSwitchToMasterMsg(true);
static AosSwitchToMasterFinishMsg		sgAosSwitchToMasterFinishMsg(true);
static AosReSwitchToMasterMsg			sgAosReSwitchToMasterMsg(true);
static AosSyncUnfinishTrans				sgAosSyncUnfinishTrans(true);

static AosConnMsgAck					sgAosConnMsgAck(true);
static AosConnMsgGetWaitSeq				sgAosConnMsgGetWaitSeq(true);
static AosConnMsgWaitSeq				sgAosConnMsgWaitSeq(true);

static AosTransAckMsg					sgAosTransAckMsg(true);
static AosTransRespTrans				sgAosTransRespTrans(true);
static AosTransSvrDeathMsg				sgAosTransSvrDeathMsg(true);
static AosStartProcessMsg				sgAosStartProcessMsg(true);
static AosAddClusterMsg					sgAosAddClusterMsg(true);
static AosStopProcessMsg				sgAosStopProcessMsg(true);
static AosAddServerMsg					sgAosAddServerMsg(true);
//static AosSetProcStatusMsg				sgAosSetProcStatusMsg(true);

static AosGetTotalFileInfoTrans			sgAosGetTotalFileInfoTrans(true);
static AosRecoverFileTrans				sgAosRecoverFileTrans(true);

static AosDiskBadMsg                    sgDiskBadMsg(true);
static AosSvrUpMsg						sgSvrUpMsg(true);
static AosSvrDeathMsg					sgSvrDeathMsg(true);
static AosProcUpMsg						sgProcUpMsg(true);
static AosProcDeathMsg					sgProcDeathMsg(true);

static AosStartDynamicProcMsg			sgStartDynamicProcMsg(true);
static AosStartProcFinishMsg			sgStartProcFinishMsg(true);
static AosKillProcMsg					sgKillProcMsg(true);
static AosGetCrtMastersMsg				sgGetCrtMastersMsg(true);
static AosSendCrtMastersMsg				sgSendCrtMastersMsg(true);
static AosGetClusterConfigMsg			sgGetClusterConfigMsg(true);

static AosStartJobMgrMsg				sgStartJobMgrMsg(true);		// Ketty 2014/03/07
static AosCheckStartJobSvrMsg			sgCheckStartJobSvrMsg(true);// Jozhi 2014/09/09

static AosTestTrans						sgTestTrans(true);		// Ketty 2014/07/15
static AosNotifyProcIsUp				sgNotifyProcIsUp(true);		// Ketty 2014/07/15

}

