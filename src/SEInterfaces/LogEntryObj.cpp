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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/LogEntryObj.h"

AosLogEntryObj::AosLogEntryObj(const int version)
:
AosJimo(AosJimoType::eLogEntry, version)
{
}


AosLogEntryObj::~AosLogEntryObj()
{
}

