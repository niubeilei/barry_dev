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
// 2014/09/16, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_CharsetMgr_h
#define Aos_SEUtil_CharsetMgr_h

#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

class AosCharsetMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosCharsetMgr();

	static AosCharsetMgr *getCharsetMgr();

	AosCharset * getCharset(AosRundata *rdata, const OmnString &name);
};

#endif
