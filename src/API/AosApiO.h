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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiO_h
#define AOS_API_ApiO_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/MemoryChecker.h"


extern OmnFilePtr AosOpenStorageFile(
		const int virtual_id, 
		const OmnString &file_key,
		const bool create_flag, 
		AosRundata *rdata AosMemoryCheckDecl);
#endif
