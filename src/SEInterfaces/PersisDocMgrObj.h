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
// 01/20/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_PersisDocMgrObj_h
#define Aos_SEInterfaces_PersisDocMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosPersisDocMgrObj : virtual public OmnRCObject
{
private:
	static AosPersisDocMgrObjPtr		smPersisDocMgr;

public:
	static void setPersisDocMgr(const AosPersisDocMgrObjPtr &d) {smPersisDocMgr = d;}
	static AosPersisDocMgrObjPtr getPersisDocMgr() {return smPersisDocMgr;}
};
#endif
