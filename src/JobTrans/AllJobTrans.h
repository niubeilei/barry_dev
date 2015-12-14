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
#ifndef Aos_JobTrans_AllJobTrans_h
#define Aos_JobTrans_AllJobTrans_h


#include "JobTrans/SendCmdTrans.h"
//#include "JobTrans/SendTaskDataTrans.h"
//#include "StreamEngine/SendStreamDataTrans.h"
#include "JobTrans/ScheduleTaskTrans.h"
#include "JobTrans/StartTaskTrans.h"
#include "JobTrans/UpdateJobProcNumTrans.h"
#include "JobTrans/UpdateTaskProgressTrans.h"
#include "JobTrans/UpdateTaskStatusTrans.h"
#include "JobTrans/UpdateTaskOutPutTrans.h"
#include "JobTrans/UpdateTaskSnapShotsTrans.h"
#include "JobTrans/AppendFileTrans.h"
#include "JobTrans/DirIsGoodTrans.h"
#include "JobTrans/FileIsGoodTrans.h"
#include "JobTrans/GetFileLengthTrans.h"
#include "JobTrans/GetDirListTrans.h"
#include "JobTrans/GetFileListTrans.h"
#include "JobTrans/GetFileInfoTrans.h"
#include "JobTrans/GetLogicPidTrans.h"
#include "JobTrans/ReadFileTrans.h"
#include "JobTrans/WriteFileTrans.h"
#include "JobTrans/ProcMsgTrans.h"
#include "JobTrans/AsyncReadFileTrans.h"
#include "JobTrans/DeleteFileTrans.h"
#include "JobTrans/GetFileListByAssignExtTrans.h"
#include "JobTrans/RemoteCmdTrans.h"
#include "JobTrans/GetTaskRunnerInfoTrans.h"
#include "JobTrans/NotifyJobTaskStopTrans.h"
#include "JobTrans/NotifyTaskMgrGetDataColOutput.h"
#include "JobTrans/NotifyTaskMgrCreateDataCol.h"  	// Barry Niu, 2015/02/12
#include "JobTrans/CreateDataCol.h"  	// Barry Niu, 2015/02/12
#include "JobTrans/StopTaskProcessTrans.h"
#include "JobTrans/UnicomReadFileTrans.h"
#include "JobTrans/CreateRaidFileTrans.h"			// Ken Lee,2014/08/15
#include "JobTrans/CreateTempFileTrans.h"			// Ken Lee,2014/08/15
#include "JobTrans/GetLocalFileInfoTrans.h"			// Ken Lee,2014/08/15
#include "JobTrans/CheckDiskFreeSizeTrans.h"
#include "JobTrans/GetFileListByDataCol.h"			// Andy zhang, 2015/01/05
#include "JobTrans/GetRunTimeValueTrans.h"			// Jozhi , 2015/01/19
#include "JobTrans/GetRunTimeValueByJobTrans.h"		// Jozhi , 2015/01/19
#include "JobTrans/SetRunTimeValueTrans.h"			// Jozhi , 2015/01/19
#include "JobTrans/SetRunTimeValueByJobTrans.h"		// Jozhi , 2015/01/19
#include "JobTrans/SendClearIILEntryMapTrans.h"     // Jozhi, 2015/03/20
#include "JobTrans/ClearTaskIILEntryMapTrans.h"     // Jozhi, 2015/03/20
#include "JobTrans/ClearCubeIILEntryMapTrans.h"     // JKen Lee,2015/12/04
#include "JobTrans/StartServiceTrans.h"     		// Jozhi, 2015/03/20
#include "JobTrans/GetServiceInfoTrans.h"
#include "JobTrans/UpdateRDDStatusTrans.h"
#include "JobTrans/JobTestTrans.h"
#include "JobTrans/JobAsyncTestTrans.h"
#include "JobTrans/DataSender.h"
#include "JobTrans/CreateDataReceiver.h"

class AosAllJobTrans
{
public:
	static void init();
};
#endif
