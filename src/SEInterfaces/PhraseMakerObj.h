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
// 2013/05/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_PhraseMakerObj_h
#define Aos_SEInterfaces_PhraseMakerObj_h

#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
using namespace std;

class AosPhraseMakerObj : virtual public OmnRCObject
{
private:

public:
	virtual OmnString create(
						const AosRundataPtr &rdata,
			            const AosXmlTagPtr &def) = 0;
};
#endif

