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
#include "DocTrans/AllDocTrans.h"

//#include "DocTrans/BatchDelDocsTrans.h"
#include "DocTrans/BatchFixedTrans.h"
#include "DocTrans/BatchCSVTrans.h"
#include "DocTrans/BatchInsertCommonDocTrans.h"
#include "DocTrans/BatchInsertDocTrans.h"
#include "DocTrans/BatchUpdateDocTrans.h"
#include "DocTrans/BatchDeleteCSVTrans.h"
#include "DocTrans/BatchDeleteGroupDocTrans.h"
//#include "DocTrans/BatchGetDocsTrans.h"
//#include "DocTrans/BatchSendStatusTrans.h"
//#include "DocTrans/BatchSendDocidsTrans.h"
//#include "DocTrans/BatchGetDocsResp.h"
#include "DocTrans/CheckDocLockTrans.h"
#include "DocTrans/CreateDocSafeTrans.h"
#include "DocTrans/CreateDocTrans.h"
#include "DocTrans/DeleteObjTrans.h"
#include "DocTrans/DocLockTrans.h"
#include "DocTrans/GetDocTrans.h"
#include "DocTrans/GetCSVDocTrans.h"
#include "DocTrans/GetCommonDocTrans.h"
#include "DocTrans/GetFixedDocTrans.h"
#include "DocTrans/IncrememtValueTrans.h"
#include "DocTrans/IsDocDeletedTrans.h"
#include "DocTrans/ModifyObjTrans.h"
#include "DocTrans/NotifyToClientTrans.h"
#include "DocTrans/SaveToFileTrans.h"
#include "DocTrans/BinaryDocAsyncTrans.h"
#include "DocTrans/BatchGetDocsAsyncTrans.h"
#include "DocTrans/DocTesterTrans.h"
#include "DocTrans/GetDocTesterTrans.h"
#include "DocTrans/IndexMgrModifyDocTrans.h"
#include "DocTrans/CreateDocSnapshotTrans.h"
#include "DocTrans/MergeDocSnapshotTrans.h"
#include "DocTrans/CommitDocSnapshotTrans.h"
#include "DocTrans/RollbackDocSnapshotTrans.h"
#include "DocTrans/BatchCreateDocTrans.h"
//#include "DocTrans/GetStatBinaryDocidTrans.h"
//#include "DocTrans/AddStatBinaryDocidTrans.h"
#include "DocTrans/BatchGetMultiFixedDocAsyncTrans.h"
#include "DocTrans/BatchGetFixedDocAsyncTrans.h"
#include "DocTrans/BatchGetNormDocAsyncTrans.h"
#include "DocTrans/BatchGetCSVDocAsyncTrans.h"
#include "DocTrans/BatchGetCommonDocAsyncTrans.h"
#include "DocTrans/ReadLockDocTrans.h"
#include "DocTrans/ReadUnLockDocTrans.h"
#include "DocTrans/WriteLockDocTrans.h"
#include "DocTrans/WriteUnLockDocTrans.h"
#include "DocTrans/BatchAddSmallDocTrans.h"
#include "DocTrans/GetSmallDocTrans.h"
#include "DocTrans/AgingSmallDocTrans.h"
#include "DocTrans/TestTrans.h"
#include "DocTrans/AsyncTestTrans.h"

//AosAllDocTrans gAosAllDocTrans;

//AosAllDocTrans::AosAllDocTrans()
void
AosAllDocTrans::init()
{
// DocTrans.
static AosBatchFixedTrans				sgBatchFixedTrans(true);
static AosBatchCSVTrans					sgBatchCSVTrans(true);
static AosBatchInsertCommonDocTrans		sgBatchInsertCommonDocTrans(true);
static AosBatchInsertDocTrans			sgBatchInsertDocTrans(true);
static AosBatchDeleteCSVTrans			sgBatchDeleteCSVTrans(true);
static AosBatchDeleteGroupDocTrans		sgBatchDeleteGroupDocTrans(true);
static AosCheckDocLockTrans				sgCheckDocLockTrans(true);
static AosCreateDocSafeTrans			sgCreateDocSafeTrans(true);
static AosCreateDocTrans				sgCreateDocTrans(true);
static AosDeleteObjTrans				sgDeleteObjTrans(true);
static AosDocLockTrans					sgDocLockTrans(true);
static AosGetDocTrans					sgGetDocTrans(true);
static AosGetFixedDocTrans				sgGetFixedDocTrans(true);
static AosGetCSVDocTrans				sgGetCSVDocTrans(true);
static AosGetCommonDocTrans				sgGetCommonDocTrans(true);
static AosIsDocDeletedTrans				sgIsDocDeletedTrans(true);
static AosModifyObjTrans				sgModifyObjTrans(true);
static AosNotifyToClient				sgNotifyToClient(true);
static AosSaveToFileTrans				sgSaveToFileTrans(true);
static AosBinaryDocAsyncTrans			sgBinaryDocAsyncTrans(true);
static AosBatchGetDocsAsyncTrans        sgBatchGetDocsAsyncTrans(true);
static AosDocTesterTrans				sgDocTesterTrans(true);
static AosGetDocTesterTrans				sgGetDocTesterTrans(true);
static AosIndexMgrModifyDocTrans		sgIndexMgrModifyDocTrans(true);
static AosCreateDocSnapshotTrans		sgCreateDocSnapshotTrans(true);
static AosMergeDocSnapshotTrans			sgMergeDocSnapshotTrans(true);
static AosCommitDocSnapshotTrans		sgCommitDocSnapshotTrans(true);
static AosRollbackDocSnapshotTrans		sgRollbackDocSnapshotTrans(true);
static AosBatchCreateDocTrans			sgBatchCreateDocTrans(true);
static AosBatchGetFixedDocAsyncTrans	sgBatchGetFixedDocAsyncTrans(true);
static AosBatchGetMultiFixedDocAsyncTrans	sgBatchGetMultiFixedDocAsyncTrans(true);
static AosBatchGetNormDocAsyncTrans		sgBatchGetNormDocAsyncTrans(true);
static AosBatchGetCSVDocAsyncTrans		sgBatchGetCSVDocAsyncTrans(true);
static AosBatchGetCommonDocAsyncTrans	sgBatchGetCommonDocAsyncTrans(true);
static AosReadLockTrans					sgReadLockTrans(true);
static AosReadUnLockTrans				sgReadUnLockTrans(true);
static AosWriteLockTrans				sgWriteLockTrans(true);
static AosWriteUnLockTrans				sgWriteUnLockTrans(true);
static AosBatchAddSmallDocTrans			sgBatchAddSmallDocTrans(true);
static AosGetSmallDocTrans				sgGetSmallDocTrans(true);
static AosAgingSmallDocTrans            sgAgingSmallDocTrans(true);
static AosBatchUpdateDocTrans			sgBatchUpdateDocTrans(true);
static AosDocTestTrans					sgDocTestTrans(true);
static AosAsyncTestTrans				sgAsyncTestTrans(true);
}

