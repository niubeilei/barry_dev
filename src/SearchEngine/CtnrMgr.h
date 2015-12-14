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
// 08/13/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngine_CtnrMgr_h
#define AOS_SearchEngine_CtnrMgr_h

#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


struct AosCtnrMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosCtnrMgr();
	~AosCtnrMgr();

	bool createSysCtnr(const OmnString &siteid, const OmnString &ctnr_objid);

private:
	bool createSysBackCtnr(const OmnString &siteid);
};

#endif
