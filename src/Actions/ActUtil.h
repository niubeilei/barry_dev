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
// 2011/01/12	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActUtil_h
#define Aos_SdocAction_ActUtil_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "Security/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

class AosActUtil
{
public:
	static AosXmlTagPtr	getDoc(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);
};
#endif

