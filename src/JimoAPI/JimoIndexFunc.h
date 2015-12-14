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
// 2015/04/20 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoIndexFunc_h
#define Aos_JimoAPI_JimoIndexFunc_h


class JimoIndexFunc
{
public:
	enum E
	{
		eInvalid,

		eGetDocidByObjid,
		eBindObjid,
		eUnBindObjid,
		eAddStrValueDoc,
		eAddStrValueDocByID,
		eAddStrValueDocByName,
		eRemoveStrValueDoc,
		eRemoveStrValueDocByID,
		eRemoveStrValueDocByName,
		eAddU64ValueDoc,
		eAddU64ValueDocByID,
		eAddU64ValueDocByName,
		eRemoveU64ValueDoc,
		eRemoveU64ValueDocByID,
		eRemoveU64ValueDocByName,
		eGetDocidBySiteidObjid,
		eIncrementKeyedValue,
		eGetDocidByIILIDOpr,
		eGetDocidByIILNameOpr,
		eGetDocidByIILIDDft,
		eGetDocidByIILNameDft,
		eBindCloudid,
		eUnBindCloudid,
		eReBindCloudid,
		eIncrementDocidToTable,
		eIncrementInlineSearchValue,
		eGetDocidByCloudid,
		eGetCtnrMemDocidByOneKey,
		eGetCtnrMemDocidByTwoKey,
		eAddHitDoc,
		eNextId1,
		eAddU64ValueDocToTableByID,
		ePreQuerySafeByIILName,
		ePreQuerySafeByIILID,
		eQuerySafeByIILName,
		eRemoveHitDoc,
		eAddHitDocByTrans,
		eAddStrValueDocByTrans,
		eAddStrValueDocToTable,
		eAddU64ValueDocByTrans,
		eRemoveHitDocByTrans,
		eRemoveStrValueDocByIDTrans,
		eRemoveStrValueDocByNameTrans,
		eRemoveU64ValueDocByIDTrans,
		eRemoveU64ValueDocByNameTrans,
		eRemoveU64DocByIDValue,
		eRemoveU64DocByNameValue,
		eModifyStrValueDoc,
		eModifyStrValueDocByID,
		eModifyStrValueDocByName,
		eModifyU64ValueDoc,
		eGetIILID,
		eCounterRangeByID,
		eCounterRangeByName,
		eRebuildBitmap,
		eQuerySafeByIILID,
		eQuerySafeByNameSnap,
		eCreateTablePublicByID,
		eCreateTablePublicByIDName,
		eSetU64ValueDocUnique,
		eSetU64ValueDocUniqueToTable,
		eSetStrValueDocUnique,
		eSetStrValueDocUniqueToTable,
		eIncrementDocidStrByID,
		eIncrementDocidStrByName,
		eIncrementDocidU64ByID,
		eIncrementDocidU64ByName,
		eCreateIILPublicByID,
		eCreateIILPublicByIDName,
		eGetDocidsByKeysIILID,
		eGetDocidsByKeysIILName,
		eDeleteIILByID,
		eDeleteIILByName,
		eGetSplitValue,
		eResetKeyedValue,
		eBatchAdd,
		eStrBatchAdd,
		eStrBatchDel,
		eStrBatchInc,
		eMergeSnapshot,
		eCreateSnapshot,
		eCommitSnapshot,
		eRollBackSnapshot,
		eGetTotalNumDocsByName,
		eGetTotalNumDocsByIDName,
		eStrBatchAddMerge,
		eStrBatchIncMerge,
		eHitBatchAdd,
		eU64BatchAdd,
		eJimoTableBatchAdd,
		eQuerySafeByNameOpr,
		eQuerySafeByIDOpr,
		eRebindObjid,
		eBitmapQueryNewSafe,
		eBitmapRsltQuerySafe,
		eMoveManualOrder,
		eRemoveManualOrder,
		eSwapManualOrder,
		eAppendManualOrder,
		eGetDocids,
		eAddInlineSearchValue,
		eModifyInlineSearchValue,
		eRemoveInlineSearchValue,
		eGetBatchDocids,
		eUpdateKeyedValue,
		eRemoveStrFirstValueDoc,
		eAddU64ValueDocToTableByName,

		eLastEntry
	};
};

#endif
