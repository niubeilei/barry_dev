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
//
// Modification History:
// 2015/03/31 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoDocFunc_h
#define Aos_JimoAPI_JimoDocFunc_h


class JimoDocFunc
{
public:
	enum E
	{
		eInvalid,

		//For DocEngine
		eGetDocByName,
		eAddReq,
		eAppendBinaryDoc,
		eBatchDataReceived,
		eBatchGetDocs,
		eBindCloudid_Doc,   
		eBindObjid_Doc,
		eCreateDoc,
		eCreateDoc1,
		eCreateDocSafe1,
		eCreateDocSafe3,
		eCreateBinaryDoc,
		eCreateBinaryDocByVid,
		eCreateJQLTable,	
		eCreateDocByJQL, 
		eCreateSnapshot_Doc,
		eCommitSnapshot_Doc,
		eCreateRootCtnr,
		eCreateDocByTemplate1,
		eCloneDoc,
		eDoesObjidExist,
		eDeleteBinaryDocByObjid,
		eDeleteBinaryDocByDocid,
		eDeleteObj,
		eDropJQLTable, 
		eDeleteDocByDocid,	
		eDeleteDocByObjid,
		eDeleteDoc,
		eGetAccessRcd,
		eGetDoc,
		eGetDocidByCloudid_Doc,
		eGetDocByObjid,
		eGetDocidByObjid_Doc,
		eGetDocByU64Docid,
		eGetDocByStrDocid,
		eGetDocByCloudid,
		eGetUserAcct,
		eGetDeletedDoc,
		eGetObjidByDocid,
		eGetCloudid,
		eGetParentContainer,
		eGetTableNames,
		eGetJQLDocs,
		eGetJQLDocidByTablename,
		eGetDocByJQL,	
		eGetDocByDocid,
		eGetParentContainerByObjid,	
		eGetParentContainerByDocid,	
		eGetDocByKey1,
		eGetDocByKey2,
		eGetDocByKeys,
		eIsCloudidBound,
		eIsShowLog,
		eIncrementValue,
		eIsLog,
		eIsRepairing,
		eModifyAttrStr1,
		eModifyAttrStr,
		eModifyAttrU64,
		eModifyAttrXPath,
		eModifyBinaryDocByStat,
		eModifyObj,
		eModifyObjAdd,
		eMergeSnapshot_Doc,
		eProcDocLock,
		eProcNotifyMsg,
		ePurgeDocByObjid,
		ePurgeDocBydocid,
		ePurgeDocByCloudid,
		eWriteLockDoc,
		eWriteLockDocByTime,
		eWriteUnlockDocByDocid,
		eWriteLockDocByObjid,
		eWriteAccessDoc,
		eReleaseWriteLock,
		eReleaseWriteLockDocByObjid,
		eReleaseWriteLockDocByDocid,
		eRetrieveBinaryDocByStat,
		eRetrieveBinaryDocByObjid,
		eRetrieveBinaryDocByDocid,
		eRetrieveBinaryDocByCaller,
		eRollbackSnapshot,
		eReadLockDocByDocid,
		eReadUnlockDocByDocid,
		eAddCreateLogRequest,
		eAddDeleteLogRequest,

		//For DocStore
		eCreateDatalet,
		eCreateDatalets,
		eCreateDataletSafe,
		eGetDataletByObjid,
		eDeleteDatalet,
		eWriteAccessDatalet,
		eBatchSaveStatDatalets,
		eRetrieveBinaryDatalet,
		eGetDataletByDocid,
		eIsDataletDeleted,
		eModifyDatalet,
		eWriteUnLockDatalet,
		eWriteLockDatalet,
		eReadUnLockDatalet,
		eReadLockDatalet,
		eMergeSnapshot,
		eCommitSnapshot,
		eRollbackSnapshotOnCube,
		eCreatebackSnapshot,
		eCheckDataletLock,
		eProcDataletLock,
		eBatchGetDatalets,
		eGetCSVDatalet,
		eGetFixedDatalet,
		eCreateSnapshot,
		eCreateSE,

		eLastEntry
	};
};

#endif
