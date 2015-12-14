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
// 2015/01/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/StreamEngineObj.h"

#include "alarm_c/alarm.h"
#include "Debug/ExitHandler.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"


AosStreamEngineObj::AosStreamEngineObj(const int version)
:
AosJimo(AosJimoType::eStreamEngine, version)
{
}


AosStreamEngineObj::~AosStreamEngineObj()
{
}

