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
// 2013/12/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SchemaMgrObj_h
#define Aos_SEInterfaces_SchemaMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/SchemaID.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSchemaMgrObj : virtual public OmnRCObject
{
private:
	static AosSchemaMgrObjPtr 	smSchemaMgr;

public:
	virtual u64 getSchemaDocid(const OmnString &type) = 0;

	static AosSchemaMgrObjPtr getSchemaMgr() {return smSchemaMgr;}
	static void setSchemaMgr(const AosSchemaMgrObjPtr &o) {smSchemaMgr = o;}
};
#endif
