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
#ifndef Aos_SEInterfaces_BmpSecMgrObj_h
#define Aos_SEInterfaces_BmpSecMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBmpSecMgrObj : virtual public OmnRCObject
{
	OmnDefineRCObject
public:
	enum
	{
		eDftMaxSectionId = 0xffff
	};

private:
	static AosBmpSecMgrObjPtr	smObject;
	static u64					smMaxSectionId;

public:
	static void setObject(const AosBmpSecMgrObjPtr &d) {smObject = d;}
	static AosBmpSecMgrObjPtr getObject() {return smObject;}

	static u64 getMaxSectionId() {return smMaxSectionId;}
	static bool isValidSectionId(const u64 &section_id) 
	{
		return section_id <= smMaxSectionId;
	}
};

#endif
