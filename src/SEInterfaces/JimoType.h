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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_JimoType_h
#define Aos_Jimo_JimoType_h

#define AOS_JIMOTYPE_INVALID				"jimo_invalid"
#define AOS_JIMOTYPE_ACCESS_CONTROL			"jimo_accessctlr"
#define AOS_JIMOTYPE_ACTION					"jimo_action"
#define AOS_JIMOTYPE_DATA_ASSEMBLER			"jimo_dataassm"
#define AOS_JIMOTYPE_DATA_CONVERTER			"jimo_dataconverter"
#define AOS_JIMOTYPE_DATA_PROC				"jimo_dataproc"
#define AOS_JIMOTYPE_DATA_PROC_ENGINE		"jimo_dataprocengine"
#define AOS_JIMOTYPE_DATA_CUBE 				"jimo_datacube"
#define AOS_JIMOTYPE_DATA_ENGINE			"jimo_dataengine"
#define AOS_JIMOTYPE_DATA_QUERY				"jimo_dataquery"
#define AOS_JIMOTYPE_DATA_READER			"jimo_datareader"
#define AOS_JIMOTYPE_DATA_RECEIVER			"jimo_datareceiver"
#define AOS_JIMOTYPE_DATA_CONVERTER			"jimo_dataconverter"
#define AOS_JIMOTYPE_DATA_RECORD			"jimo_datarecord"
#define AOS_JIMOTYPE_DATA_SPLITTER			"jimo_datasplitter"
#define AOS_JIMOTYPE_DATASET				"jimo_dataset"
#define AOS_JIMOTYPE_DUMMY					"jimo_dummy"
#define AOS_JIMOTYPE_EXPR					"jimo_expr"
#define AOS_JIMOTYPE_JOB					"jimo_job"
#define AOS_JIMOTYPE_JQL_STATEMENT			"jimo_jqlstmt"
#define AOS_JIMOTYPE_QUERY_PROC				"jimo_queryproc"
#define AOS_JIMOTYPE_QUERY_CACHER			"jimo_querycacher"
#define AOS_JIMOTYPE_QUERY_OPTIMIZER		"jimo_queryoptimizer"
#define AOS_JIMOTYPE_RECORDSET				"jimo_recordset"
#define AOS_JIMOTYPE_SCHEDULE				"jimo_schedule"
#define AOS_JIMOTYPE_TASK_SCHEDULER			"jimo_taskscheduler"
#define AOS_JIMOTYPE_SCHEDULER				"jimo_scheduler"
#define AOS_JIMOTYPE_SCHEMA					"jimo_schema"
#define AOS_JIMOTYPE_SCHEMA_PICKER			"jimo_schema_picker"
#define AOS_JIMOTYPE_DATA_SCANNER			"jimo_data_scanner"
#define AOS_JIMOTYPE_VALUE					"jimo_value"
#define AOS_JIMOTYPE_VALUESET				"jimo_valueset"
#define AOS_JIMOTYPE_STRUCT_PROC            "jimo_struct_proc"
#define AOS_JIMOTYPE_INDEXMGR				"jimo_indexmgr"
#define AOSJIMOTYPE_SYSDEFVALUE				"jimo_sysdefval"
// #define AOSJIMOTYPE_QUERYFUNC				"jimo_query_func"
// #define AOSJIMOTYPE_QUERYVAR				"jimo_query_var"
// #define AOSJIMOTYPE_QUERYTERM				"jimo_query_term"
#define AOS_JIMOTYPE_DATALET_VALIDATOR		"jimo_datalet_validator"
#define AOS_JIMOTYPE_VALIDATE_RULE			"jimo_validate_rule"
#define AOS_JIMOTYPE_SYSDEFVALUE			"jimo_sysdefval"
#define AOS_JIMOTYPE_SELECTINTO				"jimo_selectinto"
// #define AOS_JIMOTYPE_QUERYFUNC				"jimo_query_func"
// #define AOS_JIMOTYPE_QUERYVAR				"jimo_query_var"
// #define AOS_JIMOTYPE_QUERYTERM				"jimo_query_term"

class AosJimoType
{
public:
	enum E
	{
		eInvalid, 

		eAccessControl,
		eAction,
		eDataAssembler,
		eDataConverter,
		eDataProc,
		eDataProcEngine,
		eDataConnector,
		eDataEngine,
		eDataQuery,
		eDataReader,

		eDataReceiver,
		eDataRecord,
		eDataSplitter,
		eDataset,
		eDataScanner,
		eDummy,
		eExpr,
		eJob,
		eJQLStatement,
		eQueryCacher,

		eQueryProc,
		eQueryOptimizer,
		eRecordset,
		eSchedule,
		eScheduler,
		eTaskScheduler,
		eSchema,
		eSchemaPicker,
		eValue,
		eValueset,

		//eStructProc,
		eIndexMgr,
		eSysDefinedValue,
		eQueryFunc,
		eQueryVar,
		eQueryTerm,
		eDatalet,
		eTable,
		eBitmapStorageMgr,
		eBitmapEngine,
		eQueryEngine,

		eBitmapTreeMgr,
		eFunction,
		eBitmapQueryTerm,
		eBitmapInstrRunner,
		eTransBitmapQueryAgent,
		eJimoLogic,					// Chen Ding, 2014/07/26
		eJimoProg,					// Chen Ding, 2014/07/26
		eJimoLogicNew,				// Arvin Jiang, 2015/04/19
		eJimoParser,				// arvin jiang, 2015/04/19
		eJimoProgMgr,				// arvin jiang, 2015/04/19
		eJQLParser,					// Chen Ding, 2014/07/26
		eLogParser,					// Chen Ding, 2014/07/26
		ePattern,					// Chen Ding, 2014/08/03

		eDataField,					// Chen ding, 2014/08/16
		eRandomGen,					// xianghui,  2014/09/02
		eBsonField,					// Chen ding, 2014/08/17
		eDataletValidator,			// Chen ding, 2014/09/01
		eValidateRule,				// Chen ding, 2014/09/01
		eAggrFunc,					// Ketty 2014/08/19
		eAggrFuncMgr,				// Phil 2015/02/08
		eGenericVerb,				// Chen Ding, 2014/10/26 
		eGenericVerbMgr,			// Chen Ding, 2014/10/26 
		eGenericObj,				// Chen Ding, 2014/10/26 
		eGenericObjMgr,				// Chen Ding, 2014/10/26 

		eGenericMethod,				// Chen Ding, 2014/10/26 
		eDbTrans,					// Chen Ding, 2014/10/27 
		eDbTransMgr,				// Chen Ding, 2014/10/27 
		eThrifClientMgr,			// Felicia, 2014/10/29
		eLogEntry,					// Chen Ding, 2014/11/10
		eVirtualField,				// Chen Ding, 2014/11/16
		eDataAnalyzer,				// Chen Ding, 2014/11/19
		eReplicatorMgr,				// White Wu,  2014/11/19
		eProfiler,					// Chen Ding, 2014/11/20
		eDataFront,					// Chen Ding, 2014/11/25
		eLogger,					// Chen Ding, 2014/11/30
		eJimoFunction,				// Chen Ding, 2014/12/05

		eDataProcIndex,
		eDataProcMerge,
		eDataProcIndexU64,          //Barry Niu, 2015/04/20
		eDataProcUpdateIndex,
		eDataProcDocBatchOpr,
		eDataProcIILBatchOpr,
		eDataProcStr2StrMap,
		eDataProcStr2NumMap,
		eDataProcDistributionMap,
		eDataProcSwitchCase,		//Barry 2015/05/15
		eDataProcSelect,			//Barry Niu, 2015/03/05
		eDataProcGroupBy,
		eDataProcStatJoin,
		eDataProcStatDoc,			// yang
		eDataProcWordCount,			// Barry 2015/03/04
		eDataProcMinus,				// Barry 2015/03/12
		eDataProcJoin,				// Young 2015/04/09
		eDataProcLeftJoin,			// Jackie 2015/08/12
		eDataProcRankOver,			// Barry 2015/03/17
		eDataProcPip,				// Barry 2015/04/23
		eDataProcStream,			// yang
		eDataProcStreamTest1,		// yang
		eDataProcIf,				// Barry 2015/03/25
		eDataProcSyncher,			// Andy 2015/11/04
		eDataProcOnNetOffNet,		// Barry 2015/12/13

		eSyncDataProc,			// Young, 2015/09/19
		eSyncDataProcIndex,			// Young, 2015/09/19
		eSyncDataProcIILMap,		// Young, 2015/10/23
		eDataProcTransTorturer,		// Andy 2015/12/07
		eDatasetDir,				// Young 2015/01/03
		eDatasetFile,				// Rain 2015/06/13
		eDatasetIIL,				// Young 2015/01/24
		eDatasetStream,				// Phil 2015/02/23
        eDataProcDistinct,          // YanYan 2015/06/16    
		eDataProcMatch,             // YanYan 2015/06/23
		eJimoCallPackage,			// Chen Ding, 2014/12/18
		eJimoExample,				// Chen Ding, 2015/01/15
		eTorturer,					// Chen Ding, 2015/01/22
		eSyncher,					// Chen Ding, 2015/01/25
		eSyncEngine,				// Chen Ding, 2015/01/25
		eSyncJob,					// Chen Ding, 2015/01/26
		eStatistics,				// Chen Ding, 2015/02/06
		eStatModel,					// Chen Ding, 2015/02/06
		eStatMeasure,				// Chen Ding, 2015/02/06
		eDataParser,				// Chen Ding, 2015/02/12

		eClusterMgr,
		eCubeMapIDOSvr,
		eIDO,
		eIDOServer,					// Chen Ding, 2015/03/28
		eDocEngine,
		
		eDataletParser,
		eDocEngineProc,
		eDocStoreProc,

		eStreamEngine,				// Phil, 2015/03/16
		eStreamDPIndex,				// Phil, 2015/01/29
		eStreamDPIILBatchOpr,		// Phil, 2015/01/29
		eStreamDPDocBatchOpr,		// Phil, 2015/01/29
		eStreamDPGroupBy,			// Levi
		eStreamDPStatJoin,			// Levi
		eStreamDPStatDoc,			// Levi
		eStreamDPIILJoin,			// Levi
		eStreamDPSelect,			// Levi
		eDataProcIILJoin,			// Levi
		eDataProcSelectInto,		// Jozhi 
		eJPSyntaxCheck,

		eQueryFormatCSV,			//Barry  2015/09/11
		eQueryInto,					//Barry	 2015/09/11

		eDatasetConverter,			// Jozhi, 2015/09/10, JIMODB-734
		eMaxJimoTypeEntry
	};
};
#endif 
