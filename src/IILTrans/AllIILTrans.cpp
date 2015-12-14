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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/AllIILTrans.h"

//AosAllIILTrans gAosAllIILTrans;

//AosAllIILTrans::AosAllIILTrans()
void
AosAllIILTrans::init()
{
// Ketty 2013/03/29
//static AosIILTransBitmapQueryByPhysical		sgIILTransBitmapQueryByPhysical(true);
static AosIILTransCreateIIL						sgIILTransCreateIIL(true);
static AosIILTransDeleteIIL						sgIILTransDeleteIIL(true);
static AosIILTransGetSplitValueByName			sgIILTransGetSplitValueByName(true);
static AosIILTransGetTotalNumDocsByName			sgIILTransGetTotalNumDocsByName(true);
static AosIILTransRebuildBitmap					sgIILTransRebuildBitmap(true);
//static AosIILTransHitAddDocById 				sgIILTransHitAddDocById(true);
static AosIILTransHitAddDocByName 				sgIILTransHitAddDocByName(true);
//static AosIILTransHitRemoveDocById 			sgIILTransHitRemoveDocById(true);
static AosIILTransHitRemoveDocByName 			sgIILTransHitRemoveDocByName(true);
static AosIILTransPreQuery						sgIILTransPreQuery(true);
static AosIILTransBitmapQueryDocByName          sgIILTransBitmapQueryDocByName(true);
static AosIILTransBitmapRsltQueryDocByName      sgIILTransBitmapRsltQueryDocByName(true);
static AosIILTransQueryDoc						sgIILTransQueryDoc(true);
static AosIILTransStrAddInlineSearchValue		sgIILTransStrAddInlineSearchValue(true);
static AosIILTransStrAddValueDoc				sgIILTransStrAddValueDoc(true);
static AosIILTransStrBatchAdd					sgIILTransStrBatchAdd(true);
static AosIILTransStrBatchDel					sgIILTransStrBatchDel(true);
static AosIILTransStrBatchInc					sgIILTransStrBatchInc(true);
static AosIILTransStrBatchAddMerge				sgIILTransStrBatchAddMerge(true);
static AosIILTransStrBatchIncMerge				sgIILTransStrBatchIncMerge(true);
static AosIILTransStrBindCloudid				sgIILTransStrBindCouldid(true);
static AosIILTransStrBindObjid					sgIILTransStrBindObjid(true);
static AosIILTransStrUnbindCloudid				sgIILTransStrUnbindCouldid(true);
static AosIILTransStrUnbindObjid				sgIILTransStrUnbindObjid(true);
static AosIILTransStrGetDocidById				sgIILTransStrGetDocidById(true);
static AosIILTransStrGetDocidByName				sgIILTransStrGetDocidbyName(true);
static AosIILTransStrGetDocidsByKeys			sgIILTransStrGetDocidsbyKeys(true);
static AosIILTransStrGetSplitValue				sgIILTransStrGetSplitValue(true);
static AosIILTransStrIncrementDocidById			sgIILTransStrIncrementDocidById(true);
static AosIILTransStrIncrementDocidByName		sgIILTransStrIncrementDocidByName(true);
static AosIILTransStrIncrementInlineSearchValue	sgIILTransStrIncrementInlineSearchValue(true);
static AosIILTransStrModifyInlineSearchValue	sgIILTransStrModifyInlineSearchValue(true);
static AosIILTransStrModifyValueDoc 			sgIILTransStrModifyValueDoc(true);
static AosIILTransStrRemoveFirstValueDocByName	sgIILTransStrRemoveFirstValueDocByName(true);
static AosIILTransStrRemoveInlineSearchValue	sgIILTransStrRemoveInlineSearchValue(true);
static AosIILTransStrRemoveValueDocById			sgIILTransStrRemoveValueDocById(true);
static AosIILTransStrRemoveValueDocByName		sgIILTransStrRemoveValueDocByName(true);
static AosIILTransStrSetValueDocUniqueById		sgIILTransStrSetValueDocUniqueById(true);
static AosIILTransU64AddAncestor				sgIILTransU64AddAncestor(true);
static AosIILTransU64BatchAdd					sgIILTransU64BatchAdd(true);
static AosIILTransU64AddDescendant				sgIILTransU64AddDescendant(true);
static AosIILTransU64AddValueDoc				sgIILTransU64AddValueDoc(true);
static AosIILTransU64GetAncestor				sgIILTransU64GetAncestor(true);
static AosIILTransU64GetDocidById				sgIILTransU64GetDocidById(true);
static AosIILTransU64GetDocidByName				sgIILTransU64GetDocidByName(true);
static AosIILTransU64IncrementDocidById			sgIILTransU64IncrementDocidById(true);
static AosIILTransU64IncrementDocidByName		sgIILTransU64IncrementDocidByName(true);
static AosIILTransU64ModifyValueDocById 		sgIILTransU64ModifyValueDocById(true);
static AosIILTransU64ModifyValueDocByName		sgIILTransU64ModifyValueDocByName(true);
static AosIILTransU64RemoveAncestor				sgIILTransU64RemoveAncestor(true);
static AosIILTransU64RemoveDescendant			sgIILTransU64RemoveDescendant(true);
static AosIILTransU64RemoveValueDocById			sgIILTransU64RemoveValueDocById(true);
static AosIILTransU64RemoveValueDocByName		sgIILTransU64RemoveValueDocByName(true);
static AosIILTransU64SetValueDocUniqueById		sgIILTransU64SetValueDocUniqueById(true);
static AosIILTransGetIILID						sgIILTransGetIILID(true);
static AosIILTransCounterRange					sgIILTransCounterRange(true);
//static AosIILTransDistQueryDocByName			sgIILTransDistQueryDocByName(true);
static AosIILTransCreateSnapShot				sgIILTransCreateSnapShot(true);
static AosIILTransMergeSnapShot					sgIILTransMergeSnapShot(true);
static AosIILTransCommitSnapShot				sgIILTransCommitSnapShot(true);
static AosIILTransRollBackSnapShot				sgIILTransRollBackSnapShot(true);
static AosIILTransHitBatchAdd					sgIILTransHitBatchAdd(true);
static AosIILBatchGetDocidsAsyncTrans			sgAosIILBatchGetDocidsAsyncTrans(true);
static AosIILTransQueryDocByNameAsync           sgIILTransQueryDocByNameAsync(true);

// Ketty tester.
//static AosIILTransHitRlbTesterCheck				sgIILTransHitRlbTesterCheck(true);

static AosIILTransJimoTableBatchAdd				sgIILTransJimoTableBatchAdd(true);

static AosIILTransAddValueDoc  				    sgIILTransAddValueDoc(true);
static AosIILTransBatchAdd  			  		sgIILTransBatchAdd(true);
static AosIILTransBatchDel  					sgIILTransBatchDel(true);
static AosIILTransGetDocid         				sgIILTransGetDocid(true);
static AosIILTransIncrementDocid   				sgIILTransIncrementDocid(true);
static AosIILTransModifyValueDoc   				sgIILTransModifyValueDoc(true);
static AosIILTransRemoveValueDoc   				sgIILTransRemoveValueDoc(true);

static AosIILTransGetMapValues  				sgIILTransGetMapValues(true);
static AosIILTestTrans							sgIILTestTrans(true);
static AosAsyncIILTestTrans						sgAsyncIILTestTrans(true);

}

