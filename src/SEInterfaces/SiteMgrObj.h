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
// 2013/02/11	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_SiteMgrObj_h
#define AOS_SEInterfaces_SiteMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosSiteMgrObj : virtual public OmnRCObject
{
protected:
	static AosSiteMgrObjPtr		smSiteMgr;	

public:
	virtual u64 getDftSiteId() const = 0;

	static void setSiteMgr(const AosSiteMgrObjPtr &mgr){ smSiteMgr = mgr; };
	static AosSiteMgrObjPtr getSiteMgr(){ return smSiteMgr; };
};
#endif
