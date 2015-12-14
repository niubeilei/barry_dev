////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SingletonObjId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SingletonClass_SingletonObjId_h
#define Omn_SingletonClass_SingletonObjId_h



class OmnSingletonObjId
{
public:
	enum E
	{
		//
		// Group 1
		// They do not dependent on anyone
		//
		eAlarmMgr,
		eDllMgr,

		//
		// Group 2
		// They are dependent on Group 1 
		//
		eWait,
		eTimer,
		eThreadMgr,
		eMemMgr,
		eStoreMgr,
		eEventMgr,
		eNetwork,
		eNms,
		eLoggerMgr,
		eDebug,
		eTracer,

		//
		// Group 3
		//
		eTimeDriver,
		eAppMgr,
		eRetransMgr,
		eAlarmServer,
		eThreadShellMgr,

		//
		// Group 4
		//
		eSecurityMgr,
		eTransIdMgr,

		//
		// Group 5
		//
		eDbSync,
		eLicenseMgr,
		eMemMtr,
		eWebSvr,
		eAcsMgr,
		eMsgFactory,
		eDnMgr,
		eInfobusClt,
		eScheduleMgr,
		eHeartbeatMgr,
		eCapturerMgr,
		eScvsSvr,
		eScvsClt,
		eTransMgr,
		eRealmMgr,
		eTestAgent,
		eTestServer,
		eProcMgr,
		eKernelInterface,
		eCliProc,
		eCliUtilProc,
		eChannelMgr,
		eDocMgr,
		eImageClient,
		eMessageMgr,
		ePMIServer,
		eOCSPServer,
		eAttackMgr,
		eBouncerMgr,
		eStreamSimuMgr,
		eShellCommandMgr,
		eSecuredShellMgr,
		eErrorMgr,
		eObjectDict,
		eSemanticsRuntime, 
		eRuleMgr,
		eRawFilesMgr,
		eWordMgr1,
		eIILClient,
		eIILMgr,
		eIILFileMgr,
		eQueryRsltMgr,
		eDocServer,
		eDictionaryMgr,
		eSiteMgr,
		eSyncServer,
		eSysLog,
		eQueryClient,
		eQueryMgr,
		eLogMgr,
		eIdGenMgr,
		eSengAdmin,
		eBrowserSimu,
		eSessionMgr,
		eSeConfigMgr,
		eSeAdmin,
		eElemIdMgr,
		eUrlMgr,
		eUsrMgr,
		eObjMgr,
		eWebClient,
		eTagMgr,
		eIILHelper,
		eIdGenFileMgr,
		eSeIdGenMgr,
		eImageBack,
		eOnlineMgr,
		eWordMgr,
		eLockMonitorMgr,
		eUserMgr,
		eMsgClient,
		eMicroblog,
		eIMManager,
		eMsgSvrOnlineMgr,
		eMsgService,
		eWordClient,
		eWordTransClient, //ken 2011/07/19
		eIILTransClient,
		eDataSyncClt,
		eAosUserConnMgr,
		eSeSiteMgr,
		eReservedMgr,
		eIILProcMgr,
		ePython,
		eLogSvr,
		eProcServer,
		eProcService,
		eProcRegister,
		eProcControler,
		eTimerMgr, //ken 2011/6/13
		eReceiveEmail,
		eCalendarMgr,
		eCounterClt,		//CounterClient. Add by Lynch at 2011.06.10
		eQuery,
		eSendEmail,
		eVersionServer,
		eSeLogClient,
		eDocProc,
		eDocClient,
		eDocidMgr,
		eDocSvr,
		eDiskFileMgr,
		eSnapshotIdMgr,   
		eShortMsgClt,   //ShortMsgClient. Add by Brian at 2011.06.14
		eShortMsgSvr,   //ShortMsgSvr. Add by Brian at 2011.11.11
		eSyncEngineClient,	// Young, 10/16/2015
		eWorkMgr,		// Chen Ding, 08/09/2011
		eEmailClt,		
		eUserDomainMgr,	// Chen Ding, 08/27/2011
		eLocalDocMgr,	// Chen Ding, 08/30/2011
		eAccessRcdMgr,	// Chen Ding, 09/01/2011
		eUserAcctMgr,	// Chen Ding, 09/01/2011
		eDocidIdGen,
		eIILIDIdGen,
		eSystemDocMgr,	// Chen Ding, 09/05/2011
		eSysMonitor,	// Chen Ding, 09/05/2011
		eProcessMonitor,	// felicia, 2013/06/21
		eSeLogMgr,		// Chen Ding, 09/07/2011
		eIMSessionMgr,
		//eVirtualFileSysMgr,	// Ketty, 09/13/2011
		eMemoryChecker,	// Chen Ding, 10/02/2011
		eTransBktMgr,	// Chen Ding, 10/25/2011
		eSmartReplyMgr,	// Ketty, 11/03/2011
		eLangDictMgr,	// Chen Ding, 11/30/2011
		eDocLock,
		ePersisDocMgr,	// Chen Ding, 01/20/2012
		//eDiskBlockMgr,	// Chen Ding, 01/24/2012
		eDiskBlockCache,	// Chen Ding, 01/24/2012
		eFrontEndSvr,	// Chen Ding, 02/11/2012
		eRemoteBackupClt,	// Chen Ding, 02/14/2012
		eRemoteBackupSvr,	// Chen Ding, 02/14/2012
		eSoapServer,		// Chen Ding, 02/22/2012
		eSqlClient,		// Chen Ding, 02/25/2012
		eRemoteConvertData,		// Chen Ding, 02/25/2012
		eNetworkMgr,		// Chen Ding, 02/25/2012
		ePyemail,
		eSizeIdMgr,					//ken 2012/03/14
		eImportDataMgr,				//ken 2012/03/14
		eTaskMgr,					//chen 2012/04/28
		eJobMgr,					//chen 2012/04/30
		eDistributedFileMgr,		//chen 2012/07/04
		eNetFileClt,				// Ken Lee, 2014/08/06
		eNetFileMgr,				// Ken Lee, 2013/04/15
		eIILMergerMgr,			//felicia  2012/07/19
		eDataCollectorMgr,			//felicia  2012/07/19
		eDataReceiverMgr,			//Barry 2015/11/18
		//eReplicMgrClt,			//Ketty	2012/09/23
		//eReplicMgrSvr,			//Ketty	2012/09/23
		//eServerMgr,			//Ketty	2012/12/10
		eMsgServer,			//jozhi	2012/09/03
		eINotifyMgr,		//Jozhi Peng 2012/11/13
		eFileReaderMgr,			//Chen Ding 2012/10/31
		eDocBatchReaderMgr,
		eDocidShufflerMgr,
		eStoreQueryMgr,
		eQueryEngine,			// Chen Ding, 2013/01/02
		eBitmapStorageMgr,		// Chen Ding, 2013/01/08
		eStorageEngineMgr,
		ePhyLogSvr,				// Ketty 2013/03/11
		//eTransClient,			// Ketty 2013/03/11
		//eTransServer,			// Ketty 2013/03/11
		//eSvrProxy,			// Ketty 2013/04/17
		eBitmapEngine,			// Chen Ding, 2013/02/05
		eBitmapMgr,
		eBitmapTreeMgr,
		eCubeMgr,
		eWordParserEngine,    //felicia, 2013/03/29
		eCubeMessager,
		eDocidSectionMap,
		eBatchQueryReader,
		//eAosAsyncTransReqMgr, // create by Young, 08/13/2013
		eRunCmd,				// create by Young, 08/28/2013
		eSmallDocStore,			// Young, 2014/10/27
		eStorageMgrAyscIo,
		eProcessMgr,
		eLogger,
		eTaskDriver,
		eRecycle,
		eIILSave,
		eTaskTransChecker,
		eJimoAgentMonitor,
		eServiceMgr
	};
};
#endif

