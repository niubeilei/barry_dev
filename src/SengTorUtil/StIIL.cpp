////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2014/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StIIL.h"


AosStIIL::AosStIIL(
		const OmnString &iilname, 
		const u64 iilid,
		const bool key_unique,
		const bool docid_unique)
:
mIILName(iilname),
mIILID(iilid),
mKeyUnique(key_unique),
mDocidUnique(docid_unique)
{
}


AosStIIL::~AosStIIL()
{
}

