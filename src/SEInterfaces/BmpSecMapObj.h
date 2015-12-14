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
// 2013/02/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BmpSecMapObj_h
#define Aos_SEInterfaces_BmpSecMapObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBmpSecMapObj : virtual public OmnRCObject
{
private:
	enum
	{
		eMaxDocidSectionId = 0xffff
	};
	static AosBmpSecMapObjPtr		smObject;

public:
	static void setObject(const AosBmpSecMapObjPtr &d) {smObject = d;}
	static AosBmpSecMapObjPtr getObject() {return smObject;}
	static u32 maxDocidSectionId() {return eMaxDocidSectionId;}
	inline static bool isValidSectionId(const u64 section_id) 
	{
		return section_id <= eMaxDocidSectionId;
	}
	virtual int section2Physical(const u64 &section_id) = 0;
};

#endif
