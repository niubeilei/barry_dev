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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/JimoProgObj.h"

#include "API/AosApi.h"


AosJimoProgObj::AosJimoProgObj(const int version)
:
AosJimo(AosJimoType::eJimoProg, version)
{
}


AosJimoProgObj::~AosJimoProgObj()
{
}

