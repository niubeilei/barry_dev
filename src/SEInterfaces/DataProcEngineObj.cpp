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
// 2013/08/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataProcEngineObj.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"



AosDataProcEngineObj::AosDataProcEngineObj(const u32 version)
:
AosJimo(AosJimoType::eDataProcEngine, version)
{
}


AosDataProcEngineObj::~AosDataProcEngineObj()
{
}


