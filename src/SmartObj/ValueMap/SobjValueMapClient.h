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
// Modification History:
// 02/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_Map_SobjValueMapClient_h
#define Aos_SmartObj_Map_SobjValueMapClient_h

#include "SEInterfaces/ValueMapObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosSobjValueMapClient : public virtual AosValueMapObj
{
	OmnDefineRCObject;

private:

public:
	SobjValueMapClient();
	~SobjValueMapClient();

	virtual bool getMappedValue(
						const u64 &docid,
						const int64_t &key, 
						int64_t &value,
						const AosRundataPtr &rdata);
};

#endif

