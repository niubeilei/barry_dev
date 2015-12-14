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
// 	Created: 12/14/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VpdParser/GicProc.h"

#include "Debug/Debug.h"
#include "VpdParser/VpdParser.h"
#include "VpdParser/GicProcNamevalue.h"


AosGicProc::AosGicProc(const AosGicName::E id)
:
mGicId(id)
{
}


AosGicProcNamevaluePtr
AosGicProc::toNamevalue() const
{
	return 0;
}
