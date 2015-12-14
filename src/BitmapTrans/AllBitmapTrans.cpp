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
// 2013/08/22	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "BitmapTrans/AllBitmapTrans.h"

#include "BitmapTrans/TransSendInstrs.h"
#include "BitmapTrans/TransBmpUpdated.h"
#include "BitmapTrans/TransBitmapQuery.h"

// #include "BitmapTrans/TransSendCachedBuffs.h"

void
AosAllBitmapTrans::init()
{
static AosTransBmpUpdated			sgAosTransBmpUpdated(true);
static AosTransSendInstrs			sgAosTransSendInstrs(true);

static AosTransBitmapQuery			sgAosTransBitmapQuery(true);
}

