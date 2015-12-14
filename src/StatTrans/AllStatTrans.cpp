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
// 2014/01/24	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StatTrans/AllStatTrans.h"

#include "StatTrans/BatchSaveStatDocsTrans.h"
#include "StatTrans/CreateStatMetaFileTrans.h"
#include "StatTrans/BatchGetStatDocsTrans.h"
#include "StatTrans/BatchGetStatDocsTrans.h"
//#include "StatTrans/SaveStatBinaryIdTrans.h"

//AosAllStatTrans gAosAllStatTrans;

//AosAllStatTrans::AosAllStatTrans()
void
AosAllStatTrans::init()
{

static AosBatchSaveStatDocsTrans		sgBatchSaveStatDocsTrans(true);
static AosCreateStatMetaFileTrans		sgCreateStatMetaFileTrans(true);
static AosBatchGetStatDocsTrans			sgBatchGetStatDocsTrans(true);
//static AosSaveStatBinaryIdTrans		sgSaveStatBinaryIdTrans(true);

}

