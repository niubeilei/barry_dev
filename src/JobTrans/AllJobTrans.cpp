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
#include "JobTrans/AllJobTrans.h"
void
AosAllJobTrans::init()
{
static AosSendCmdTrans						sgSendCmdTrans(true);
//static AosSendTaskDataTrans					sgSendTaskDataTrans(true);
static AosScheduleTaskTrans					sgScheduleTaskTrans(true);
static AosStartTaskTrans					sgStartTaskTrans(true);
static AosUpdateJobProcNumTrans				sgUpdateJobProcNumTrans(true);
static AosUpdateTaskOutPutTrans				sgUpdateTaskOutPutTrans(true);
static AosUpdateTaskSnapShotsTrans			sgUpdateTaskSnapShotsTrans(true);
static AosUpdateTaskProgressTrans			sgUpdateTaskProgressTrans(true);
static AosUpdateTaskStatusTrans				sgUpdateTaskStatusTrans(true);
static AosAppendFileTrans					sgAppendFileTrans(true);
static AosDirIsGoodTrans					sgDirIsGoodTrans(true);
static AosFileIsGoodTrans					sgFileIsGoodTrans(true);
static AosGetFileLengthTrans				sgGetFileLengthTrans(true);
static AosGetFileListByAssignExtTrans		sgGetFileListByAssignExtTrans(true);
static AosGetDirListTrans					sgGetDirListTrans(true);
static AosGetFileListTrans					sgGetFileListTrans(true);
static AosGetFileInfoTrans					sgGetFileInfoTrans(true);
static AosGetLogicPidTrans					sgGetLogicPidTrans(true);
static AosReadFileTrans						sgReadFileTrans(true);
static AosWriteFileTrans					sgWriteFileTrans(true);
static AosProcMsgTrans						sgProcMsgTrans(true);
static AosAsyncReadFileTrans				sgAsyncReadFileTrans(true);
static AosDeleteFileTrans					sgDeleteFileTrans(true);
static AosRemoteCmdTrans					sgRemoteCmdTrans(true);
static AosGetTaskRunnerInfoTrans			sgGetTaskRunnerInfoTrans(true);
static AosNotifyJobTaskStopTrans			sgNotifyJobTaskStopTrans(true);
static AosNotifyTaskMgrGetDataColOutput		sgNotifyTaskMgrGetDataColOutput(true);
static AosNotifyTaskMgrCreateDataCol		sgNotifyTaskMgrCreateDataCol(true); // barry niu, 2015/02/12  
static AosCreateDataCol						sgCreateDataCol(true); 				// barry niu, 2015/02/12  
static AosStopTaskProcessTrans				sgStopTaskProcessTrans(true);
static AosUnicomReadFileTrans				sgUnicomReadFileTrans(true);
static AosCreateRaidFileTrans				sgCreateRaidFileTrans(true);		// Ken Lee, 2014/08/15
static AosCreateTempFileTrans				sgCreateTempFileTrans(true);		// Ken Lee, 2014/08/15
static AosGetLocalFileInfoTrans				sgGetLocalFileInfoTrans(true);		// Ken Lee, 2014/08/15
static AosGetFileListByDataCol				sgGetFileListByDataCol(true);		// Andy zhang, 2015/01/05

static AosCheckDiskFreeSizeTrans			sgCheckDiskFreeSizeTrans(true);
static AosGetRunTimeValueTrans				sgGetRunTimeValueTrans(true);
static AosGetRunTimeValueByJobTrans			sgGetRunTimeValueByJobTrans(true);
static AosSetRunTimeValueTrans				sgSetRunTimeValueTrans(true);
static AosSetRunTimeValueByJobTrans			sgSetRunTimeValueByJobTrans(true);
static AosSendClearIILEntryMapTrans         sgSendClearIILEntryMapTrans(true);
static AosClearTaskIILEntryMapTrans         sgClearTaskIILEntryMapTrans(true);
static AosClearCubeIILEntryMapTrans         sgClearCubeIILEntryMapTrans(true);
static AosStartServiceTrans					sgStartServiceTrans(true);
static AosGetServiceInfoTrans               sgGetServiceInfoTrans(true);
static AosUpdateRDDStatusTrans              sgUpdateRDDStatusTrans(true);
static AosJobAsyncTestTrans					sgJobAsyncTestTrans(true);
static AosJobTestTrans						sgJobTestTrans(true);
static AosDataSender						sgDataSender(true); 				// barry, 2015/11/18
static AosCreateDataReceiver 				sgCreateDataReceiver(true);			// barry, 2015/11/18
}

