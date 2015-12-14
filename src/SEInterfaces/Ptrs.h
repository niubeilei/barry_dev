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
// 01/06/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEInterfaces_Ptrs_h
#define Omn_SEInterfaces_Ptrs_h

#include "Util/SPtr.h"

class AosExprObj;

OmnPtrDecl(AosObjMgrObj, 			AosObjMgrObjPtr)
OmnPtrDecl(AosSmartDocObj, 			AosSmartDocObjPtr)
OmnPtrDecl(AosActionObj, 			AosActionObjPtr)
OmnPtrDecl(AosUserMgmtObj, 			AosUserMgmtObjPtr)
OmnPtrDecl(AosIDTransMapObj, 		AosIDTransMapObjPtr)
OmnPtrDecl(AosIDTransMapCaller, 	AosIDTransMapCallerPtr)
OmnPtrDecl(AosIDTransVector, 		AosIDTransVectorPtr)
OmnPtrDecl(AosIILClientObj, 		AosIILClientObjPtr)
OmnPtrDecl(AosLangDictObj, 			AosLangDictObjPtr)
OmnPtrDecl(AosDocSelObj, 			AosDocSelObjPtr)
OmnPtrDecl(AosDocMgrObj, 			AosDocMgrObjPtr)
OmnPtrDecl(AosDocSelRandObj, 		AosDocSelRandObjPtr)
OmnPtrDecl(AosDocClientObj, 		AosDocClientObjPtr)
OmnPtrDecl(AosObjMgrObj, 			AosObjMgrObjPtr)
OmnPtrDecl(AosQueryObj, 			AosQueryObjPtr)
OmnPtrDecl(AosPersisDocMgrObj, 		AosPersisDocMgrObjPtr)
OmnPtrDecl(AosVersionServerObj, 	AosVersionServerObjPtr)
OmnPtrDecl(AosSeLogClientObj, 		AosSeLogClientObjPtr)
OmnPtrDecl(AosLoginMgrObj, 			AosLoginMgrObjPtr)
// OmnPtrDecl(AosStorageMgrObj, 		AosStorageMgrObjPtr) // Chen Ding, 2013/02/11
OmnPtrDecl(AosSoapServerObj, 		AosSoapServerObjPtr)
OmnPtrDecl(AosValueMapObj, 			AosValueMapObjPtr)
OmnPtrDecl(AosConditionObj, 		AosConditionObjPtr)
OmnPtrDecl(AosSqlClientObj, 		AosSqlClientObjPtr)
OmnPtrDecl(AosSecReqObj, 			AosSecReqObjPtr)
OmnPtrDecl(AosQueryClientObj, 		AosQueryClientObjPtr)
OmnPtrDecl(AosRemoteBkCltObj, 		AosRemoteBkCltObjPtr)
OmnPtrDecl(AosStorageEngineObj, 	AosStorageEngineObjPtr)
OmnPtrDecl(AosRemoteBkCltObj, 		AosRemoteBkCltObjPtr)
OmnPtrDecl(AosCloudSvrMgrObj, 		AosCloudSvrMgrObjPtr)
OmnPtrDecl(AosNetworkMgrObj, 		AosNetworkMgrObjPtr)
OmnPtrDecl(AosDocidMgrObj, 			AosDocidMgrObjPtr)
OmnPtrDecl(AosSizeIdMgrObj, 		AosSizeIdMgrObjPtr)
OmnPtrDecl(AosNetListener, 			AosNetListenerPtr)
OmnPtrDecl(AosCounterCltObj, 		AosCounterCltObjPtr)
OmnPtrDecl(AosCounterSvrObj, 		AosCounterSvrObjPtr)
OmnPtrDecl(AosJobMgrObj, 			AosJobMgrObjPtr)
OmnPtrDecl(AosJobObj,				AosJobObjPtr)				// Chen Ding, 07/24/2012
OmnPtrDecl(AosJobCreatorObj,		AosJobCreatorObjPtr)		// Chen Ding, 07/24/2012
OmnPtrDecl(AosCommandRunnerObj,		AosCommandRunnerObjPtr)
OmnPtrDecl(AosTaskObj,				AosTaskObjPtr)
OmnPtrDecl(AosTaskCreatorObj,		AosTaskCreatorObjPtr)		// Chen Ding, 07/28/2012
OmnPtrDecl(AosIILScannerObj,		AosIILScannerObjPtr)
OmnPtrDecl(AosIILScannerListener,	AosIILScannerListenerPtr)
OmnPtrDecl(AosSecurityMgrObj,		AosSecurityMgrObjPtr)
OmnPtrDecl(AosValueSelObj,			AosValueSelObjPtr)
OmnPtrDecl(AosCombinerObj,			AosCombinerObjPtr)			// Chen Ding, 06/04/2012
OmnPtrDecl(AosPartitionerObj,		AosPartitionerObjPtr)		// Chen Ding, 06/04/2012
OmnPtrDecl(AosSorterObj,			AosSorterObjPtr)			// Chen Ding, 06/04/2012
OmnPtrDecl(AosDataCreatorObj,		AosDataCreatorObjPtr)		// Chen Ding, 06/04/2012
OmnPtrDecl(AosDataCollectorObj,		AosDataCollectorObjPtr)		// Chen Ding, 06/04/2012
OmnPtrDecl(AosCompressorObj,		AosCompressorObjPtr)		// Chen Ding, 06/04/2012
OmnPtrDecl(AosDataSenderObj,		AosDataSenderObjPtr)		// Chen Ding, 06/04/2012
//OmnPtrDecl(AosTransClientObj,		AosTransClientObjPtr)		// Chen Ding, 07/01/2012
//OmnPtrDecl(AosTransServerObj,		AosTransServerObjPtr)		// Chen Ding, 07/01/2012
OmnPtrDecl(AosTransCltObj,			AosTransCltObjPtr)			// Chen Ding, 07/01/2012
OmnPtrDecl(AosTransCallerObj,		AosTransCallerObjPtr)			// Chen Ding, 07/01/2012
OmnPtrDecl(AosTransSvrObj,			AosTransSvrObjPtr)			// Chen Ding, 07/01/2012
OmnPtrDecl(AosCubeTransSvrObj,		AosCubeTransSvrObjPtr)
OmnPtrDecl(AosDistributedFileMgrObj,AosDistributedFileMgrObjPtr)// Chen Ding, 07/04/2012
OmnPtrDecl(AosQueryContextObj,		AosQueryContextObjPtr)		// Chen Ding, 07/05/2012
OmnPtrDecl(AosDataSourceObj1,		AosDataSourceObjPtr)		// Chen Ding, 07/14/2012
OmnPtrDecl(AosDataBlobObj,			AosDataBlobObjPtr)			// Chen Ding, 07/14/2012
OmnPtrDecl(AosThreadShellRunnerMgrObj,	AosThreadShellRunnerMgrObjPtr) // Chen Ding, 07/14/2012
OmnPtrDecl(AosNetFileObj,			AosNetFileObjPtr) 			// Ken Lee, 2014/08/07
OmnPtrDecl(AosNetFileCltObj,		AosNetFileCltObjPtr) 		// Ken Lee, 2014/08/07
OmnPtrDecl(AosNetFileMgrObj,		AosNetFileMgrObjPtr) 		// Ken Lee, 2014/08/07
OmnPtrDecl(AosDataCacherObj,		AosDataCacherObjPtr) 		// Chen Ding, 07/17/2012
OmnPtrDecl(AosDataCacherCreatorObj,	AosDataCacherCreatorObjPtr)	// Chen Ding, 07/17/2012
OmnPtrDecl(AosReadCacherObj,		AosReadCacherObjPtr) 		// Chen Ding, 07/17/2012
OmnPtrDecl(AosWriteCacherObj,		AosWriteCacherObjPtr) 		// Chen Ding, 07/17/2012
OmnPtrDecl(AosDataScannerObj,		AosDataScannerObjPtr) 		// Chen Ding, 07/17/2012
OmnPtrDecl(AosDataScannerCreatorObj, AosDataScannerCreatorObjPtr) // Chen Ding, 07/17/2012
OmnPtrDecl(AosDataRecordObj,		AosDataRecordObjPtr)		// Chen Ding, 07/17/2012
OmnPtrDecl(AosTaskDataObj,			AosTaskDataObjPtr)			// Chen Ding, 07/19/2012
OmnPtrDecl(AosTaskDataCreatorObj,	AosTaskDataCreatorObjPtr)	// Ken Lee, 10/15/2012
OmnPtrDecl(AosDataFieldObj,			AosDataFieldObjPtr)			// Chen Ding, 07/21/2012
OmnPtrDecl(AosDataSamplerObj,		AosDataSamplerObjPtr)		// Felicia, 07/23/2012
OmnPtrDecl(AosDataSamplerCreatorObj, AosDataSamplerCreatorObjPtr)	// Felicia, 07/23/2012
OmnPtrDecl(AosIILObj,				AosIILObjPtr)				// Chen Ding, 07/24/2012
OmnPtrDecl(AosIILMgrObj,			AosIILMgrObjPtr)			// Chen Ding, 07/24/2012
OmnPtrDecl(AosDataCollectorObj,		AosDataCollectorObjPtr)		// Chen Ding, 07/24/2012
OmnPtrDecl(AosDataCollectorCreatorObj, AosDataCollectorCreatorObjPtr)// Chen Ding, 07/24/2012
OmnPtrDecl(AosTaskTransObj, 		AosTaskTransObjPtr)			// Chen Ding, 08/05/2012
OmnPtrDecl(AosTaskTransCreatorObj,	AosTaskTransCreatorObjPtr)	// Chen Ding, 08/05/2012
OmnPtrDecl(AosFilterObj,			AosFilterObjPtr)			// Chen Ding, 08/29/2012
OmnPtrDecl(AosFilterCreatorObj,		AosFilterCreatorObjPtr)		// Chen Ding, 08/29/2012
OmnPtrDecl(AosDocFileMgrObj,		AosDocFileMgrObjPtr)		// Chen Ding, 08/29/2012
//OmnPtrDecl(AosVirtualFileObj,		AosVirtualFileObjPtr)		// Chen Ding, 08/30/2012
//OmnPtrDecl(AosVirtualFileCreatorObj,AosVirtualFileCreatorObjPtr)// Chen Ding, 08/30/2012
//OmnPtrDecl(AosIILObj,				AosIILObjPtr)				// Shawn, 	  04/28/2013
OmnPtrDecl(AosInmemCounterObj,		AosInmemCounterObjPtr)		// Chen Ding, 09/23/2012
OmnPtrDecl(AosInmemCounterCreatorObj,AosInmemCounterCreatorObjPtr)// Chen Ding, 09/23/2012
OmnPtrDecl(AosEventObj,				AosEventObjPtr)				// Chen Ding, 09/29/2012
OmnPtrDecl(AosEventCreatorObj,		AosEventCreatorObjPtr)		// Chen Ding, 09/29/2012
OmnPtrDecl(AosFileReadListener,		AosFileReadListenerPtr)		// Chen Ding, 10/31/2012
OmnPtrDecl(AosShortMsgCltObj,		AosShortMsgCltObjPtr)		// Chen Ding, 11/06/2012
OmnPtrDecl(AosFileReadRequest,		AosFileReadRequestPtr)		// Chen Ding, 11/06/2012
OmnPtrDecl(AosQueryRsltObj,			AosQueryRsltObjPtr)			// Chen Ding, 11/22/2012
OmnPtrDecl(AosBitmapObj,			AosBitmapObjPtr)			// Chen Ding, 11/22/2012
OmnPtrDecl(AosBitmapTreeMgrObj,		AosBitmapTreeMgrObjPtr)		// Shawn
OmnPtrDecl(AosBigSQLParserObj,		AosBigSQLParserObjPtr)		// Chen Ding, 12/25/2012
OmnPtrDecl(AosBitmapStorageMgrObj,	AosBitmapStorageMgrObjPtr)	// Chen Ding, 2013/01/09
OmnPtrDecl(AosQueryEngineObj,		AosQueryEngineObjPtr)		// Chen Ding, 2013/01/13
OmnPtrDecl(AosBitmapTreeObj,		AosBitmapTreeObjPtr)		// Chen Ding, 2013/01/14
OmnPtrDecl(AosServerInfo, 			AosServerInfoPtr)			// Ketty 2013/01/22
OmnPtrDecl(AosCubeGroup, 			AosCubeGroupPtr)			// Ketty 2013/01/22
OmnPtrDecl(AosVfsMgrObj, 			AosVfsMgrObjPtr)			// Ketty 2013/01/23
OmnPtrDecl(AosBitmapEngineObj, 		AosBitmapEngineObjPtr)		// Chen Ding, 2013/02/08 
OmnPtrDecl(AosSiteMgrObj, 			AosSiteMgrObjPtr)			// Chen Ding, 2013/02/11
OmnPtrDecl(AosIILExecutorObj,		AosIILExecutorObjPtr)		// Chen Ding, 2013/02/14
OmnPtrDecl(AosCompIILTreeObj,		AosCompIILTreeObjPtr)		// Chen Ding, 2013/02/14
OmnPtrDecl(AosBmpExeCaller,			AosBmpExeCallerPtr)			// Chen Ding, 2013/02/17
OmnPtrDecl(AosBmpSecMapObj,			AosBmpSecMapObjPtr)	
OmnPtrDecl(AosBmpSecMgrObj,			AosBmpSecMgrObjPtr)	
OmnPtrDecl(AosPhyLogSvrObj,         AosPhyLogSvrObjPtr)         // Ketty, 2013/03/11
OmnPtrDecl(AosWordMgrObj,         	AosWordMgrObjPtr)           // Ketty, 2013/03/11
OmnPtrDecl(AosTransSvrConnObj,      AosTransSvrConnObjPtr)      // Ketty, 2013/03/21
OmnPtrDecl(AosIILQueryReqObj,		AosIILQueryReqObjPtr)	
OmnPtrDecl(AosIILQueryCacheEngObj,	AosIILQueryCacheEngObjPtr)	
OmnPtrDecl(AosQueryReqObj,			AosQueryReqObjPtr)	
OmnPtrDecl(AosQueryTermObj,			AosQueryTermObjPtr)	
OmnPtrDecl(AosCubeMgrObj,			AosCubeMgrObjPtr)	
OmnPtrDecl(AosIILCacheEngineObj,	AosIILCacheEngineObjPtr)	
OmnPtrDecl(AosDocidSectionMapObj,	AosDocidSectionMapObjPtr)	
OmnPtrDecl(AosCapplet,				AosCappletPtr)	
OmnPtrDecl(AosDatalet,				AosDataletPtr)	
OmnPtrDecl(AosDocClientCaller,		AosDocClientCallerPtr)	
OmnPtrDecl(AosBitmapTransObj,		AosBitmapTransObjPtr)	
OmnPtrDecl(AosAioCaller,			AosAioCallerPtr)	
OmnPtrDecl(AosActionCaller,			AosActionCallerPtr)	
OmnPtrDecl(AosDataProcObj,			AosDataProcObjPtr)			// Chen Ding, 2013/05/04
OmnPtrDecl(AosDLLDataProcObj,		AosDLLDataProcObjPtr)		// Chen Ding, 2013/05/04
OmnPtrDecl(AosDLLObj,				AosDLLObjPtr)				// Chen Ding, 2013/05/04
OmnPtrDecl(AosDataEngineTaskObj,	AosDataEngineTaskObjPtr)	// Chen Ding, 2013/05/04
OmnPtrDecl(AosDataAssemblerObj,		AosDataAssemblerObjPtr)		// Chen Ding, 2013/05/04
OmnPtrDecl(AosEntryCreatorObj,		AosEntryCreatorObjPtr)		// Chen Ding, 2013/05/04
OmnPtrDecl(AosDataCalcObj,			AosDataCalcObjPtr)			// Chen Ding, 2013/05/05
OmnPtrDecl(AosDLLActionObj,			AosDLLActionObjPtr)			// Chen Ding, 2013/05/05
OmnPtrDecl(AosDLLCondObj,			AosDLLCondObjPtr)			// Chen Ding, 2013/05/06
OmnPtrDecl(AosBitmapMgrObj,			AosBitmapMgrObjPtr)	
OmnPtrDecl(AosIpcCltObj,			AosIpcCltObjPtr)			// Ketty, 2013/05/09	
OmnPtrDecl(AosSvrProxyObj,			AosSvrProxyObjPtr)			// Ketty, 2013/07/18	
OmnPtrDecl(AosBackupLogObj,			AosBackupLogObjPtr)			// Chen Ding, 2013/05/17
OmnPtrDecl(AosPhraseMakerObj,		AosPhraseMakerObjPtr)		// Chen Ding, 2013/05/25
//OmnPtrDecl(AosJobSplitterObj,		AosJobSplitterObjPtr)		// Chen Ding, 2013/06/02
OmnPtrDecl(AosSchedulerObj,			AosSchedulerObjPtr)			// Chen Ding, 2013/06/02
OmnPtrDecl(AosBuffData,				AosBuffDataPtr)			 
OmnPtrDecl(AosLogCaller,			AosLogCallerPtr)			 
OmnPtrDecl(AosDatasetObj,			AosDatasetObjPtr)			 
OmnPtrDecl(AosFmtMgrObj,			AosFmtMgrObjPtr)			// Ketty, 07/16/2013
OmnPtrDecl(AosMsgServerObj,			AosMsgServerObjPtr)			 
OmnPtrDecl(AosMsgClientObj,			AosMsgClientObjPtr)			 
OmnPtrDecl(AosTaskMgrObj,			AosTaskMgrObjPtr)			 
OmnPtrDecl(AosServiceMgrObj,		AosServiceMgrObjPtr)			 
OmnPtrDecl(AosSchemaSelectorObj,	AosSchemaSelectorObjPtr)			 
OmnPtrDecl(AosQueryProcObj,			AosQueryProcObjPtr)			 
OmnPtrDecl(AosRlbTesterSvrObj,		AosRlbTesterSvrObjPtr)		// Ketty, 2013/08/12	
OmnPtrDecl(AosCommandProcObj,		AosCommandProcObjPtr)			 
OmnPtrDecl(AosDataConverterObj,		AosDataConverterObjPtr)			 
OmnPtrDecl(AosDataReceiverObj,		AosDataReceiverObjPtr)			 
OmnPtrDecl(AosDataProcEngineObj,	AosDataProcEngineObjPtr)			 
OmnPtrDecl(AosReplicMgrObj,			AosReplicMgrObjPtr)			 
OmnPtrDecl(AosTransCallerObj,		AosTransCallerObjPtr)			 
OmnPtrDecl(AosSmallDocStoreObj,		AosSmallDocStoreObjPtr)		// Young, 2013/10/25
OmnPtrDecl(AosSchemaObj,			AosSchemaObjPtr)			 
OmnPtrDecl(AosRecordsetObj,			AosRecordsetObjPtr)			 
OmnPtrDecl(AosRunnableObj,			AosRunnableObjPtr)			// Jackie, 2013/10/25
OmnPtrDecl(AosScheduleObj,			AosScheduleObjPtr)			// Jackie, 2013/10/25
OmnPtrDecl(AosSchemaPickerObj,		AosSchemaPickerObjPtr)			 
OmnPtrDecl(AosDataReaderObj,		AosDataReaderObjPtr)		// Chen Ding, 2013/11/08
OmnPtrDecl(AosDllMgrObj,			AosDllMgrObjPtr)			// Chen Ding, 2013/11/09
OmnPtrDecl(AosDatasetObj,			AosDatasetObjPtr)			// Chen Ding, 2013/11/09
OmnPtrDecl(AosDataFileObj,			AosDataFileObjPtr)			// Chen Ding, 2013/11/21
OmnPtrDecl(AosDataSplitterObj,		AosDataSplitterObjPtr)		// Chen Ding, 2013/11/24
OmnPtrDecl(AosDataConnectorObj,		AosDataConnectorObjPtr)			// Chen Ding, 2013/11/24
OmnPtrDecl(AosQueryProcCallback,	AosQueryProcCallbackPtr)			 
OmnPtrDecl(AosCachedObject,			AosCachedObjectPtr)			 
OmnPtrDecl(AosExprObj,				AosExprObjPtr)			 
OmnPtrDecl(AosSchemaMgrObj,			AosSchemaMgrObjPtr)			 
OmnPtrDecl(AosDataParserMgrObj,		AosDataParserMgrObjPtr)			 
OmnPtrDecl(AosJqlStmtObj,			AosJqlStmtObjPtr)			 
OmnPtrDecl(AosJqlStmtQueryObj,		AosJqlStmtQueryObjPtr)			 
OmnPtrDecl(AosBitmapQueryTermObj,	AosBitmapQueryTermObjPtr)			 
OmnPtrDecl(AosQueryCaller,			AosQueryCallerPtr)
OmnPtrDecl(AosQueryDistrMapObj, 	AosQueryDistrMapObjPtr)
OmnPtrDecl(AosMySqlRecordObj,		AosMySqlRecordObjPtr)			 
OmnPtrDecl(AosDataEngineListener,	AosDataEngineListenerPtr)			 
OmnPtrDecl(AosDataEngineObj,		AosDataEngineObjPtr)			 
OmnPtrDecl(AosQueryRsltCacherObj,	AosQueryRsltCacherObjPtr)			 
OmnPtrDecl(AosQueryOptimizerObj,	AosQueryOptimizerObjPtr)			 
OmnPtrDecl(AosDataConnectorCallerObj,	AosDataConnectorCallerObjPtr)			 
OmnPtrDecl(AosValueObj,				AosValueObjPtr)			 
OmnPtrDecl(AosConfigMgrObj,			AosConfigMgrObjPtr)			 
OmnPtrDecl(AosQueryCacherObj,		AosQueryCacherObjPtr)			 
OmnPtrDecl(AosSqlTableObj,			AosSqlTableObjPtr)			 
OmnPtrDecl(AosDataOutputHandler,	AosDataOutputHandlerPtr)	// Ketty 2013/12/19	
OmnPtrDecl(AosIndexMgrObj,			AosIndexMgrObjPtr)	 		// felicia, 2014/01/17
OmnPtrDecl(AosSengineImportDocObj, 	AosSengineImportDocObjPtr)	// Ken Lee, 2014/01/13
OmnPtrDecl(AosSengineDocReqObj, 	AosSengineDocReqObjPtr)	// Ken Lee, 2014/01/13
OmnPtrDecl(AosParalIILObj, 			AosParalIILObjPtr)			// Chen Ding, 2014/01/28
OmnPtrDecl(AosSysDefValObj, 		AosSysDefValObjPtr)			// Chen Ding, 2014/01/30
OmnPtrDecl(AosQueryFuncObj, 		AosQueryFuncObjPtr)			// Chen Ding, 2014/01/30
OmnPtrDecl(AosQueryVarObj, 			AosQueryVarObjPtr)			// Chen Ding, 2014/01/30
OmnPtrDecl(AosStatQueryAnalyzerObj,	AosStatQueryAnalyzerObjPtr)	// Ketty, 2014/02/14
OmnPtrDecl(AosStatSvrObj,			AosStatSvrObjPtr)			// Ketty, 2014/02/21
OmnPtrDecl(AosStatCltObj,			AosStatCltObjPtr)			// Ketty, 2014/02/21
OmnPtrDecl(AosBmpBlockIndexObj,		AosBmpBlockIndexObjPtr)		// Chen Ding, 2014/04/12
OmnPtrDecl(AosBitmapExecutorSvrObj,	AosBitmapExecutorSvrObjPtr)	// Chen Ding, 2014/04/12
OmnPtrDecl(AosDfmSnapshotObj, 		AosDfmSnapshotObjPtr)		// Ketty, 2014/03/03
OmnPtrDecl(AosBitmapInstrRunnerObj, AosBitmapInstrRunnerObjPtr)	// Ketty, 2014/03/03
OmnPtrDecl(AosTransBitmapQueryAgentObj , AosTransBitmapQueryAgentObjPtr)	// Ketty, 2014/03/03
OmnPtrDecl(AosStructProcObj, 		AosStructProcObjPtr)
OmnPtrDecl(AosJimoProgObj, 			AosJimoProgObjPtr)			// Chen Ding, 	2014/07/26
OmnPtrDecl(AosJimoLogicObj, 		AosJimoLogicObjPtr)			// Chen Ding, 	2014/07/26
OmnPtrDecl(AosJimoLogicObjNew, 		AosJimoLogicObjNewPtr)		// Chen Ding, 	2015/05/26
OmnPtrDecl(AosJQLParserObj, 		AosJQLParserObjPtr)			// Chen Ding, 	2014/07/26
OmnPtrDecl(AosLogParserObj, 		AosLogParserObjPtr)			// Chen Ding, 	2014/07/26
OmnPtrDecl(AosAggrFuncObj, 			AosAggrFuncObjPtr)			// Chen Ding, 	2014/08/12
OmnPtrDecl(AosAggrFuncMgrObj, 		AosAggrFuncMgrObjPtr)		// Phil, 	2015/02/08
OmnPtrDecl(AosQueryFilterObj, 		AosQueryFilterObjPtr)		// Ken Lee, 	2014/08/19
OmnPtrDecl(AosDataletValidatorObj,	AosDataletValidatorObjPtr)	// Chen Ding, 	2014/09/01
OmnPtrDecl(AosValidateRuleObj,		AosValidateRuleObjPtr)		// Chen Ding, 	2014/09/01
OmnPtrDecl(AosDataSortObj,			AosDataSortObjPtr)			// Ken Lee, 	2014/09/23
OmnPtrDecl(AosGroupQueryObj,		AosGroupQueryObjPtr)		// Shawn Li 	2014/10/19 
OmnPtrDecl(AosGenericVerbObj,		AosGenericVerbObjPtr)		// Chen Ding, 	2014/10/26
OmnPtrDecl(AosGenericVerbMgrObj,	AosGenericVerbMgrObjPtr)	// Chen Ding, 	2014/10/26
OmnPtrDecl(AosGenericMethodObj,		AosGenericMethodObjPtr)		// Chen Ding, 	2014/10/26
OmnPtrDecl(AosGenericObj,			AosGenericObjPtr)			// Chen Ding, 	2014/10/26
OmnPtrDecl(AosGenericObjMgrObj,		AosGenericObjMgrObjPtr)		// Chen Ding, 	2014/10/26
OmnPtrDecl(AosGenericValueObj,		AosGenericValueObjPtr)		// Chen Ding, 	2014/11/16
OmnPtrDecl(AosDbTransObj,			AosDbTransObjPtr)			// Chen Ding, 	2014/10/27
OmnPtrDecl(AosLoggerObj,			AosLoggerObjPtr)			// Chen Ding, 	2014/11/08
OmnPtrDecl(AosVirtualFieldObj,		AosVirtualFieldObjPtr)		// Chen Ding, 	2014/11/16
OmnPtrDecl(AosDataAnalyzerObj,		AosDataAnalyzerObjPtr)		// Chen Ding, 	2014/11/19
OmnPtrDecl(AosReplicatorMgrObj,		AosReplicatorMgrObjPtr)		// White Wu, 	2014/11/19
OmnPtrDecl(AosProfilerObj,			AosProfilerObjPtr)			// Chen Ding, 	2014/11/20
OmnPtrDecl(AosReplicatorMgrObj,		AosReplicatorMgrObjPtr)		// Chen Ding, 	2014/11/19
OmnPtrDecl(AosWordParserObj,		AosWordParserObjPtr)		// Chen Ding, 	2014/12/06
OmnPtrDecl(AosLogicTaskObj,			AosLogicTaskObjPtr)			// Jozhi 2014/12/30
OmnPtrDecl(AosDocSvrObj,            AosDocSvrObjPtr)
OmnPtrDecl(AosUserAcctObj,			AosUserAcctObjPtr)			// Ken Lee, 2015/01/08
OmnPtrDecl(AosUserDomainObj,		AosUserDomainObjPtr)		// YaZong Ma, 2015/01/08
OmnPtrDecl(AosDocSvrCbObj,			AosDocSvrCbObjPtr)			// YaZong Ma, 2015/01/14
OmnPtrDecl(AosTestMgrObj,			AosTestMgrObjPtr)			// YaZong Ma, 2015/01/08
OmnPtrDecl(AosSyncherObj,			AosSyncherObjPtr)			// Chen Ding, 2015/01/25
OmnPtrDecl(AosSyncEngineClientObj,	AosSyncEngineClientObjPtr)	// Young, 2015/10/19
OmnPtrDecl(AosSyncEngineObj,		AosSyncEngineObjPtr)		// Chen Ding, 2015/01/25
OmnPtrDecl(AosSyncJobObj,			AosSyncJobObjPtr)			// Chen Ding, 2015/01/28
OmnPtrDecl(AosStreamEngineObj,		AosStreamEngineObjPtr)		// Chen Ding, 2015/01/29
OmnPtrDecl(AosDataSyncObj,			AosDataSyncObjPtr)			// Ketty, 2015/03/16
OmnPtrDecl(AosStatisticsObj,		AosStatisticsObjPtr)		// Chen Ding, 2015/02/06
OmnPtrDecl(AosStatModelObj,			AosStatModelObjPtr)			// Chen Ding, 2015/02/06
OmnPtrDecl(AosStatMeasureObj,		AosStatMeasureObjPtr)		// Chen Ding, 2015/02/06
OmnPtrDecl(AosSequenceMgrObj,		AosSequenceMgrObjPtr)		// rain, 2015/02/11
OmnPtrDecl(AosDataParserObj,		AosDataParserObjPtr)		// Chen Ding, 2015/02/12
OmnPtrDecl(AosDocidMgrObj,			AosDocidMgrObjPtr)
OmnPtrDecl(AosJimoParserObj,		AosJimoParserObjPtr)		// arvin jiang,2015/04/19		
OmnPtrDecl(AosJimoProgMgrObj,		AosJimoProgMgrObjPtr)		// arvin jiang,2015/04/19		
OmnPtrDecl(AosClusterObj,			AosClusterObjPtr)			// Chen Ding, 2015/03/14
OmnPtrDecl(AosClusterMgrObj,		AosClusterMgrObjPtr)		// Chen Ding, 2015/03/14
OmnPtrDecl(AosCubeMapObj,			AosCubeMapObjPtr)			// Chen Ding, 2015/03/16
OmnPtrDecl(AosJimoCaller,			AosJimoCallerPtr)			// YaZong Ma, 2015/03/19
OmnPtrDecl(AosSeqnoIDOObj,			AosSeqnoIDOObjPtr)			// YaZong Ma, 2015/03/29
OmnPtrDecl(AosU64IDOObj,			AosU64IDOObjPtr)			// YaZong Ma, 2015/03/29
OmnPtrDecl(AosIDOJimoProgObj,		AosIDOJimoProgObjPtr)		// Chen Ding, 2015/04/11
OmnPtrDecl(AosDataletParserObj,		AosDataletParserObjPtr)		// Chen Ding, 2015/04/11
OmnPtrDecl(AosDocEngineProcObj,		AosDocEngineProcObjPtr)		// Chen Ding, 2015/04/11
OmnPtrDecl(AosDocStoreProcObj,		AosDocStoreProcObjPtr)		// Chen Ding, 2015/04/11
OmnPtrDecl(AosDataletSEObj,			AosDataletSEObjPtr)		// Chen Ding, 2015/04/11
OmnPtrDecl(AosDfmDoc,				AosDfmDocPtr)		// Chen Ding, 2015/04/11
OmnPtrDecl(AosJPSyntaxCheckerObj,	AosJPSyntaxCheckerObjPtr)	
OmnPtrDecl(AosQueryFormatObj,		AosQueryFormatObjPtr)	
OmnPtrDecl(AosQueryIntoObj,			AosQueryIntoObjPtr)	

#endif
