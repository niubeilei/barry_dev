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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterface_SysInfo_H
#define AOS_SEInterface_SysInfo_H

#define SYSDB "_sysdb"
#define SYSTABLE_JOBSTAT "_jobStat"
#define SYSTABLE_JOBDATA "_jobData"
#define SYSTABLE_JOBDATASTAT "_jobDataStat"
#define SYSTABLE_JIMOACTORS "_jimoActors"
#define SYSUSER_ROOT "_root"

//for jobDataStat table DATASTATs
#define JOBDATASTAT_SERVICE "ServiceName"
#define JOBDATASTAT_JOBNAME "JobName"
#define JOBDATASTAT_JOBID   "JobId"
#define JOBDATASTAT_TASKID	"TaskId"
#define JOBDATASTAT_ACTNAME "ActName"
#define JOBDATASTAT_ASMNAME "AsmName"
#define JOBDATASTAT_DATAID	"DataId"
#define JOBDATASTAT_INOUT 	"InOut"
#define JOBDATASTAT_PKTS 	"TotalPkts"
#define JOBDATASTAT_BYTES 	"TotalBytes"
#define JOBDATASTAT_PHYID 	"PhyId"
#define JOBDATASTAT_TIME 	"UpdateTime"
#define JOBDATASTAT_PROCTIME	"ProcTime"

//for jobStat information
#define JOBSTAT_JOBID	"JobId"
#define JOBSTAT_TASKID	"TaskId"
#define JOBSTAT_CYCLES	"Cycles"
#define JOBSTAT_AVETIME    "AveTime"

//for jobData information
#define JOBDATA_DATAID 	   "DataId"
#define JOBDATA_STATUS 	   "Status"
#define JOBDATA_IDUID 	   "IDUId"
#define JOBDATA_RDDID 	   "RDDId"
#define JOBDATA_SEGID 	   "SegId"
#define JOBDATA_DATAPROCID "DataProcId"
#define JOBDATA_SENDTASKID "SendTaskId"
#define JOBDATA_SENDDATAPROC "SendDataProcId"
#define JOBDATA_SENDPHYID  "SendPhyId"
#define JOBDATA_RECVDATAPROC "RecvDataProcId"
#define JOBDATA_RECVTASKID "RecvTaskId"
#define JOBDATA_RECVPHYID  "RecvPhyId"
#define JOBDATA_PROCTIME   "ProcTime"

//for jimoActors information
#define JIMOACTORS_NAME 	"Name"
#define JIMOACTORS_CLASSNAME 	"ClassName"
#define JIMOACTORS_CLASSPATH 	"ClassPath"
#define JIMOACTORS_TYPE_LANGUAGE 	"Type_language"
#define JIMOACTORS_DESCRIPTION 	"Description"


#endif
