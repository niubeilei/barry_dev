////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SearchEngine_DocServerCb_h
#define AOS_SearchEngine_DocServerCb_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosDocServerCb : virtual public OmnRCObject
{
public:
	virtual void procFinished(
		const bool status, 
		const u64 &docId,
		const OmnString &errmsg,
		const void *userdata, 
		const int numWords) = 0;
};

#endif
#endif


