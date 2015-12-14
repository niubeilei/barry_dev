#ifndef Omn_JimoLogicNew_Ptrs_h
#define Omn_JimoLogicNew_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosJimoLogicAddStat,      AosJimoLogicAddStatPtr)
OmnPtrDecl(AosJimoLogicStat,         AosJimoLogicStatPtr)
OmnPtrDecl(AosJimoLogicCreateJob,    AosJimoLogicCreateJobPtr)
OmnPtrDecl(AosJimoLogicCreateProcedure,    AosJimoLogicCreateProcedurePtr)
OmnPtrDecl(AosJimoLogicRunJob,    AosJimoLogicRunJobPtr) //Gavin 2015/07/24
OmnPtrDecl(AosJimoLogicRunProcedure,    AosJimoLogicRunProcedurePtr) //Gavin 2015/07/24
OmnPtrDecl(AosJimoLogicProcedure,    AosJimoLogicProcedurePtr)
OmnPtrDecl(AosJimoLogic,             AosJimoLogicPtr)
OmnPtrDecl(AosJimoLogicCreateStat,   AosJimoLogicCreateStatPtr)
OmnPtrDecl(AosJimoLogicDataProcStat,   AosJimoLogicDataProcStatPtr)	//bryant
OmnPtrDecl(AosJimoLogicDataProcDistinct,   AosJimoLogicDataProcDistinctPtr)	//bryant 2015-07-21
OmnPtrDecl(AosJimoLogicDataProcMatch,   AosJimoLogicDataProcMatchPtr)	//bryant 2015-07-23
OmnPtrDecl(AosJimoLogicDataProcCheckPoint,   AosJimoLogicDataProcCheckPointPtr)	//bryant 2015-09-19
OmnPtrDecl(AosJimoLogicCreateIndex,  AosJimoLogicCreateIndexPtr)
OmnPtrDecl(AosJimoLogicBeginJob,  	 AosJimoLogicBeginJobPtr)
OmnPtrDecl(AosJimoLogicEndJob, 	 	 AosJimoLogicEndJobPtr)
OmnPtrDecl(AosJimoLogicDataProcMap,  AosJimoLogicDataProcMapPtr)
OmnPtrDecl(AosJimoLogicCreateDistMap,  AosJimoLogicCreateDistMapPtr)
OmnPtrDecl(AosJimoLogicCreateDoc,  AosJimoLogicCreateDocPtr)
OmnPtrDecl(AosJimoLogicCallProcedure,  AosJimoLogicCallProcedurePtr)
OmnPtrDecl(AosJimoLogicDataProcUnion,  AosJimoLogicDataProcUnionPtr)
OmnPtrDecl(AosJimoLogicDatasetDir,  AosJimoLogicDatasetDirPtr)
OmnPtrDecl(AosJimoLogicDatasetFile,  AosJimoLogicDatasetFilePtr)
OmnPtrDecl(AosJimoLogicDataProc,  AosJimoLogicDataProcPtr)
OmnPtrDecl(AosJimoLogicDataProcIf,  AosJimoLogicDataProcIfPtr)
OmnPtrDecl(AosJimoLogicDataProcIndex,  AosJimoLogicDataProcIndexPtr)
OmnPtrDecl(AosJimoLogicDataProcIndexU64,  AosJimoLogicDataProcIndexU64Ptr)
OmnPtrDecl(AosJimoLogicDataProcJoin,  AosJimoLogicDataProcJoinPtr)
OmnPtrDecl(AosJimoLogicDataProcSelect,  AosJimoLogicDataProcSelectPtr)
OmnPtrDecl(AosJimoLogicDataProcSelectR1R2,  AosJimoLogicDataProcSelectR1R2Ptr)
OmnPtrDecl(AosJimoLogicDataProcSelectMax,  AosJimoLogicDataProcSelectMaxPtr)
OmnPtrDecl(AosJimoLogicDataProcSelectMin,  AosJimoLogicDataProcSelectMinPtr)
OmnPtrDecl(AosJimoLogicDataProcSelectAverage,  AosJimoLogicDataProcSelectAveragePtr)
OmnPtrDecl(AosJimoLogicDataProcStr2StrMap,  AosJimoLogicDataProcStr2StrMapPtr)
OmnPtrDecl(AosJimoLogicDataProcStr2NumMap,  AosJimoLogicDataProcStr2NumMapPtr)
OmnPtrDecl(AosJimoLogicDataProcMinus,  AosJimoLogicDataProcMinusPtr)
OmnPtrDecl(AosJimoLogicDataProcRankOver,  AosJimoLogicDataProcRankOverPtr)
OmnPtrDecl(AosJimoLogicDataProcSwitchCase,  AosJimoLogicDataProcSwitchCasePtr)
OmnPtrDecl(AosJimoLogicDataProcWordCount,  AosJimoLogicDataProcWordCountPtr)
OmnPtrDecl(AosJimoLogicDataProcWordCount2,  AosJimoLogicDataProcWordCount2Ptr)
OmnPtrDecl(AosJimoLogicDatasetIIL,  AosJimoLogicDatasetIILPtr) //rain 2015-06-01
OmnPtrDecl(AosJimoLogicDataProcMerge,  AosJimoLogicDataProcMergePtr)
OmnPtrDecl(AosJimoLogicCreateService,  AosJimoLogicCreateServicePtr)
OmnPtrDecl(AosJimoLogicDataProcIILJoin,  AosJimoLogicDataProcIILJoinPtr)
OmnPtrDecl(AosJimoLogicDataProcSelectStream,  AosJimoLogicDataProcSelectStreamPtr)
OmnPtrDecl(AosJimoLogicDataProcUpdateIndex,  AosJimoLogicDataProcUpdateIndexPtr)
OmnPtrDecl(AosJimoLogicDataProcPipe,  AosJimoLogicDataProcPipePtr)
OmnPtrDecl(AosJimoLogicCreateStatStream,  AosJimoLogicCreateStatStreamPtr)
OmnPtrDecl(AosJimoLogicLoaddata,  AosJimoLogicLoaddataPtr)
OmnPtrDecl(AosJimoLogicStatStream,  AosJimoLogicStatStreamPtr)
OmnPtrDecl(AosJimoLogicIndexStream,  AosJimoLogicIndexStreamPtr)
OmnPtrDecl(AosJimoLogicJoinStream,  AosJimoLogicJoinStreamPtr)
OmnPtrDecl(AosJimoLogicDataProcDoc,  AosJimoLogicDataProcDocPtr)
OmnPtrDecl(AosJimoLogicDatasetTable,  AosJimoLogicDatasetTablePtr)
OmnPtrDecl(AosJimoLogicUserVar,  AosJimoLogicUserVarPtr)
OmnPtrDecl(AosJimoLogicSetUserVar,  AosJimoLogicSetUserVarPtr)
OmnPtrDecl(AosJimoLogicIf,  AosJimoLogicIfPtr)
OmnPtrDecl(AosJimoLogicDataProcSyncher,  AosJimoLogicDataProcSyncherPtr)
OmnPtrDecl(AosJimoLogicStartTorturer,  AosJimoLogicStartTorturerPtr)
OmnPtrDecl(AosJimoLogicShowTorturer,  AosJimoLogicShowTorturerPtr)
#endif
