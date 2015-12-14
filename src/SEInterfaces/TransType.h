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
// 03/15/2013 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TransType_h
#define Aos_SEInterfaces_TransType_h

#include "Util/String.h" 

class AosTransType
{

public:
	enum E 
	{
		eInvalid,
	
		// for DocTrans.
		eBatchDelDocs,
		eBatchFixedClean,
		eBatchFixedFinish,
		eBatchFixedStart,
		eBatchFixed,
		eBatchCSV,	// Ken Lee, 2014/01/11
		eBatchDeleteCSV,	// Jozhi, 2014/12/16
		eBatchDeleteGroupDoc, //Jozhi, 2014/12/25
		eBatchGetDocs,
		eBatchSendStatus,
		eCheckDocLock,
		eCreateSafeDoc,

		eCreateDoc,
		eDeleteObj,
		eDocLock,
		eGetDoc,
		eGetCSVDoc,
		eGetFixedDoc,
		eBatchGetCSVDocAsync,
		eBatchGetFixedDocAsync,
		eBatchGetMultiFixedDocAsync,
		eBatchGetNormDocAsync,
		eIncrememtValue,
		eIsDocDeleted,
		eModifyObj,
		eSaveToFile,
		eBatchGetDocsResp,

		eBatchSendDocids,
		eNotifyToClient,

		eIndexMgrModifyDoc, // felicia, 2014/02/13
		eReadLockDoc, //felicia, 2014/10/14
		eReadUnLockDoc, //felicia, 2014/10/14
		eWriteLockDoc, //felicia, 2014/10/14
		eWriteUnLockDoc, //felicia, 2014/10/14

		// for iilTrans.
		eBitmapQueryDocByName,
		eBitmapRsltQueryDocByName,
		eCounterRange,
		eCreateIIL,
		eDeleteIIL,
		eDistQueryDocByName,
		eGetIILID,
		eGetSplitValueByName,
		eGetTotalNumDocsByName,

		eHitAddDocById,
		eHitAddDocByName,
		eHitRemoveDocById,
		eHitRemoveDocByName,
		ePreQuery,
		eQueryDoc,

		eStrAddInlineSearchValue,
		eStrAddValueDoc,
		eStrBatchAdd,
		eStrBatchDel,
		eStrBatchInc,
		eStrBatchAddMerge,
		eStrBatchIncMerge,
		eStrBindCloudid,
		eStrBindObjid,
		eStrUnbindCloudid,
		eStrUnbindObjid,

		eStrGetDocidById,
		eStrGetDocidByName,
		eStrGetDocidsByKeys,
		eStrGetSplitValue,
		eStrIncrementDocidById,
		eStrIncrementDocidByName,
		eStrIncrementInlineSearchValue,

		eStrModifyInlineSearchValue,
		eStrModifyValueDoc,
		eStrQueryColumn,
		eStrRemoveFirstValueDocByName,
		eStrRemoveInlineSearchValue,
		eStrRemoveValueDocById,
		eStrRemoveValueDocByName,
		eStrSetValueDocUniqueById,

		eU64AddAncestor,
		eU64AddDescendant,
		eU64AddValueDoc,
		eU64BatchAdd,
		eU64GetAncestor,
		eU64GetDocidById,
		eU64GetDocidByName,
		eU64IncrementDocidById,
		eU64IncrementDocidByName,
		eU64ModifyValueDocById,
		eU64ModifyValueDocByName,
		eU64RemoveAncestor,
		eU64RemoveDescendant,
		eU64RemoveValueDocById,
		eU64RemoveValueDocByName,
		eU64SetValueDocUniqueById,

		// for LogTrans
		eAddLog,
		eRetrieveLog,
		eRetrieveLogs,
		eBatchAddLog,
		eCreateLog,

		// for JobTrans
		eAbortTask,
		eEndTransaction,
		ePauseTask,
		eResumeTask,
		eRollBack,
		eRunTask,
		eSendCmd,
		eSendTaskData,
		eSendStreamData,
		eStartTask,
		eUpdateJobProcNum,
		eUpdateTaskOutPut,
		eUpdateTaskProgress,
		eUpdateTaskSnapShots,

		eUpdateTaskStatus,
		eUnicomReadFile,

		// for NetFile 
		eAppendFile,
		eDirIsGood,
		eFileIsGood,
		eGetFileLength,
		eGetDirList,
		eGetFileList,
		eGetFileInfo,
		eGetLogicPid,
		eReadFile,
		eWriteFile,
		eDeleteFile,

		// for Counter.
		eCounter,
	
		//for MsgServer
		eProcMsg,

		// for SysTrans.
		eThreadMonitor,
		eAskWheatherDeath,
		eTellServerDeath,

		// for BitmapEngine
		eBmpInstr,

		eRetrvBmp,
		eCachedBuffs,

		eCachedResp,
		eAsyncResp,	// Ketty 2013/05/14
		eAsyncReadFile,
		eAsyncReadFileCb,
		eAsyncGetFileLength,
		eAsyncGetFileLengthCb,

		eSendBitmapInstrs,
		eSendCachedBuffs,
		eCreateSnapShot, //felicia, 2013/05/14
		eCommitSnapShot, //felicia, 2013/05/14
		eRollBackSnapShot, //felicia, 2013/05/14
		eHitBatchAdd, //felicia, 2013/05/27
		
		// Create by Young
		eRemoteCmd,
		eBinaryDoc,
		eBatchAddSmallDoc,
		eGetSmallDoc,
		eAgingSmallDoc,
		
		// Ketty Sys SockConn msg.
		eConnAck,
		eGetWaitSeq,
		eWaitSeq,

		// Ketty Sys msg.
		eTransResp,
		eTransAckMsg,
		eTransSvrDeathMsg,
		eSwitchToBkp,
		eStopSendFmt,
		eSyncUnfinishTrans,
		eSetNewMasterMsg,
		eRebootProcMsg,
		eTriggerResendMsg,
		eResendEndMsg,
		eSvrUpMsg,
		eSvrDeathMsg,
		eProcUpMsg,
		eProcDeathMsg,
		eGetCrtMastersMsg,
		eSendCrtMastersMsg,
		eGetClusterConfigMsg,

		eNextSendFmtId,
		eSwitchToMasterMsg,
		eSwitchToMasterFinishMsg,
		eReSwitchToMasterMsg,
		eReadFmt,
		eGetMaxRecvedFmtId,
		eSendFinishedFmtId,
		//eFinishFmts,
		eSendFmt,
		eSvrStoppedMsg,
		eGetTotalFileInfo,
		eRecoverFile,

		eGetFileListByAssignExt,
		// Create by Andy
		eGetFileListByDataCol,
		eNotifyTaskMgrGetDataColOutput,

		//barry
		eDataSender,
		eCreateDataReceiver,

		// Ketty Fmt Tester Trans.
		eRlbTesterCreateFile,
		eRlbTesterModifyFile,
		eRlbTesterDeleteFile,
		eRlbTesterReadData,
		eRlbTesterStartProcMsg,
		eHitRlbTesterCheck,
		eRlbTesterSwitchMFinish,
		eRlbTesterIsProcDeathed,
		eRlbTesterDamageDisk,

		//Doc Trans
		eBatchGetDocsAsync,
		eGetDocTester,
		eDocTester,

		eAdminStartProcess,
		eStartProcess,
		eAddCluster,
		eStartTaskProcess,
		eAdminStopProcess,
		eStopProcess,
		eAdminAddServer,
		eAddServer,
		eAdminProcessStatus,
		eSetProcStatus,
		eGetTaskRunnerInfo,
		eStartMsg,
		eIILBatchGetDocids,
		eDiskBadMsg,
		
		eStartDynamicProc,
		eStartProcFinish,
		eKillProcMsg,
		
		eNotifyJobTaskStop,
		eStopTaskProcess,
		eScheduleTask,
		eBitmapUpdated,
		eBitmapQuery,
		eBitmapQueryResp,
		eCalcQueryResults,
		eCalcQueryResultsResp,
	
		eBatchGetStatDocs,		// Ketty 2014/01/24
		eBatchGetStatDocsNew,	// Ketty 2014/10/16
		eBatchSaveStatDocs,		// Ketty 2014/08/18
		eCreateStatMetaFile,	// Ketty 2014/08/18
		//eSaveStatBinaryId,	// Ketty 2014/07/21
		
		eRebuildBitmap,
		
		eBatchGetVt2dDocs,		// Ketty 2014/01/24

		eQueryDocByNameAsync,
		
		eStartJobMgrMsg,		// Ketty 2014/03/07
		eCheckStartJobSvrMsg,	// Jozhi 2014/09/09
		eCreateDocSnapshot,
		eCommitDocSnapshot,
		eRollbackDocSnapshot,
		eBatchCreateDoc,
		eGetStatBinaryDocid,
		eAddStatBinaryDocid,

		eMergeDocSnapshot,
		eMergeSnapShot,

		eCreateRaidFile,		// Ken Lee, 2014/08/15
		eCreateTempFile,		// Ken Lee, 2014/08/15
		eGetLocalFileInfo,		// Ken Lee, 2014/08/15
		
		eCreateSizeIdByDoc,
		eCreateSizeIdByDocid,

		eJimoTableBatchAdd,		// Ken Lee, 2014/09/24
		
		eCheckDiskFreeSize,		// Ken Lee, 2014/10/14
		eGetRunTimeValue,		// Jozhi, 2015/01/19
		eGetRunTimeValueByJob,	// Jozhi, 2015/01/19
		eSetRunTimeValue,		// Jozhi, 2015/01/19
		eSetRunTimeValueByJob,	// Jozhi, 2015/01/19
		eCreateDataCol,
		eNotifyTaskMgrCreateDataCol,
		eSendClearIILEntryMap,  // Jozhi, 2015/03/20
		eClearTaskIILEntryMap,  // Jozhi, 2015/03/20
		eClearCubeIILEntryMap,  // Ken Lee, 2015/12/04

		eNotifyProcIsUp,		//Jozhi, 2015/04/15
		eBatchInsertCommonDoc,
		eBatchInsertDoc,
		eBatchGetCommonDocAsync,
		eGetCommonDoc,
		
		eAddValueDoc,
		eRemoveValueDoc,
		eModifyValueDoc,
		eIncrementDocid,
		eGetDocid,
		eBatchDel,
		eBatchAdd,
		eStartService,
        eGetServiceInfo,
		eUpdateRDDStatus,

		eGetMapValues,
		eBatchUpdateDoc,

		eTestDocTrans, 		// Andy, 2015/11/19
		eAsyncTestDocTrans, 		// Andy, 2015/11/19
		eTestJobTrans,		// Andy, 2015/11/19
		eAsyncTestJobTrans,		// Andy, 2015/11/19
		eTestIILTrans,		// Andy, 2015/11/19
		eAsyncTestIILTrans,		// Andy, 2015/11/19

		eMax
	};

	static bool isValid(const E code) 
	{
		return code > eInvalid && code < eMax;
	}
	
};

#endif

