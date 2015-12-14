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
#ifndef Aos_IILTrans_AllIILTrans_h
#define Aos_IILTrans_AllIILTrans_h

#include "IILTrans/BitmapQueryByPhysicalTrans.h"
#include "IILTrans/BitmapQueryDocByNameTrans.h"
#include "IILTrans/BitmapRsltQueryDocByNameTrans.h"
#include "IILTrans/CreateIILTrans.h"
#include "IILTrans/DeleteIILTrans.h"
#include "IILTrans/GetSplitValueByNameTrans.h"
#include "IILTrans/GetTotalNumDocsByNameTrans.h"
#include "IILTrans/HitAddDocByIdTrans.h"
#include "IILTrans/HitAddDocByNameTrans.h"
#include "IILTrans/HitRemoveDocByIdTrans.h"
#include "IILTrans/HitRemoveDocByNameTrans.h"
#include "IILTrans/PreQueryTrans.h"
#include "IILTrans/QueryDocTrans.h"
#include "IILTrans/RebuildBitmapTrans.h"
#include "IILTrans/StrAddInlineSearchValueTrans.h"
#include "IILTrans/StrAddValueDocTrans.h"
#include "IILTrans/StrBatchAddTrans.h"
#include "IILTrans/StrBatchDelTrans.h"
#include "IILTrans/StrBatchIncTrans.h"
#include "IILTrans/StrBatchAddMergeTrans.h"
#include "IILTrans/StrBatchIncMergeTrans.h"
#include "IILTrans/StrBindCloudidTrans.h"
#include "IILTrans/StrBindObjidTrans.h"
#include "IILTrans/StrUnbindCloudidTrans.h"
#include "IILTrans/StrUnbindObjidTrans.h"
#include "IILTrans/StrGetDocidByIdTrans.h"
#include "IILTrans/StrGetDocidByNameTrans.h"
#include "IILTrans/StrGetDocidsByKeysTrans.h"
#include "IILTrans/StrGetSplitValueTrans.h"
#include "IILTrans/StrIncrementDocidByIdTrans.h"
#include "IILTrans/StrIncrementDocidByNameTrans.h"
#include "IILTrans/StrIncrementInlineSearchValueTrans.h"
#include "IILTrans/StrModifyInlineSearchValueTrans.h"
#include "IILTrans/StrModifyValueDocTrans.h"
#include "IILTrans/StrRemoveFirstValueDocByNameTrans.h"
#include "IILTrans/StrRemoveInlineSearchValueTrans.h"
#include "IILTrans/StrRemoveValueDocByIdTrans.h"
#include "IILTrans/StrRemoveValueDocByNameTrans.h"
#include "IILTrans/StrSetValueDocUniqueByIdTrans.h"
#include "IILTrans/U64AddAncestorTrans.h"
#include "IILTrans/U64AddDescendantTrans.h"
#include "IILTrans/U64AddValueDocTrans.h"
#include "IILTrans/U64BatchAddTrans.h"
#include "IILTrans/U64GetAncestorTrans.h"
#include "IILTrans/U64GetDocidByIdTrans.h"
#include "IILTrans/U64GetDocidByNameTrans.h"
#include "IILTrans/U64IncrementDocidByIdTrans.h"
#include "IILTrans/U64IncrementDocidByNameTrans.h"
#include "IILTrans/U64ModifyValueDocByIdTrans.h"
#include "IILTrans/U64ModifyValueDocByNameTrans.h"
#include "IILTrans/U64RemoveAncestorTrans.h"
#include "IILTrans/U64RemoveDescendantTrans.h"
#include "IILTrans/U64RemoveValueDocByIdTrans.h"
#include "IILTrans/U64RemoveValueDocByNameTrans.h"
#include "IILTrans/U64SetValueDocUniqueByIdTrans.h"
#include "IILTrans/GetIILIDTrans.h"
#include "IILTrans/CounterRangeTrans.h"
//#include "IILTrans/DistQueryDocByNameTrans.h"
#include "IILTrans/CreateSnapShotTrans.h"
#include "IILTrans/MergeSnapShotTrans.h"
#include "IILTrans/CommitSnapShotTrans.h"
#include "IILTrans/RollBackSnapShotTrans.h"
#include "IILTrans/HitBatchAddTrans.h"

//#include "IILTrans/HitRlbTesterCheckTrans.h"
#include "IILTrans/IILBatchGetDocidsAsyncTrans.h"
#include "IILTrans/QueryDocByNameAsyncTrans.h"

#include "IILTrans/JimoTableBatchAddTrans.h"

#include "IILTrans/AddValueDocTrans.h"   
#include "IILTrans/BatchAddTrans.h"   
#include "IILTrans/BatchDelTrans.h"   
#include "IILTrans/GetDocidTrans.h"   
#include "IILTrans/IncrementDocidTrans.h"   
#include "IILTrans/ModifyValueDocTrans.h"   
#include "IILTrans/RemoveValueDocTrans.h"   

#include "IILTrans/GetMapValuesTrans.h"   

#include "IILTrans/IILTestTrans.h"   
#include "IILTrans/AsyncIILTestTrans.h"   


class AosAllIILTrans
{
public:
	//AosAllIILTrans();
	static void init();
};
//extern AosAllIILTrans gAosAllIILTrans;

#endif
