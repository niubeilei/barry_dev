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
// 2013/03/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocidSectionMapObj_h
#define Aos_SEInterfaces_DocidSectionMapObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosDocidSectionMapObj : virtual public OmnRCObject
{
private:
	static AosDocidSectionMapObjPtr smObject;

public:
	static AosDocidSectionMapObjPtr getObject() {return smObject;}
	static void setObject(const AosDocidSectionMapObjPtr &d) {smObject = d;}
};
#endif
