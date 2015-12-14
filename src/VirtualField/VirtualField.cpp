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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VirtualField/VirtualField.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



AosVirtualField::AosVirtualField(const int version)
:
AosVirtualFieldObj(version)
{
}


AosVirtualField::~AosVirtualField()
{
}


