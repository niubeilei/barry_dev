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
// 2013/02/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Rundata/RundataParm.h"


AosRundataParm::AosRundataParm()
:
mType(eInvalidType)
{
}

AosRundataParm::AosRundataParm(const Type type)
:
mType(type)
{
}

AosRundataParm::~AosRundataParm()
{
}

