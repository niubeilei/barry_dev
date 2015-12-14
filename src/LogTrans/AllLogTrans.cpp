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
#include "LogTrans/AllLogTrans.h"

#include "LogTrans/AddLogTrans.h"
#include "LogTrans/RetrieveLogTrans.h"
#include "LogTrans/RetrieveLogsTrans.h"

//AosAllLogTrans gAosAllLogTrans;

//AosAllLogTrans::AosAllLogTrans()
void
AosAllLogTrans::init()
{

static AosAddLogTrans					sgAddLogTrans(true);
static AosRetrieveLogTrans				sgRetrieveLogTrans(true);
static AosRetrieveLogsTrans				sgRetrieveLogsTrans(true);

}

