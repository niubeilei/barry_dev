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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_VersionServerObj_h
#define Aos_SEInterfaces_VersionServerObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosVersionServerObj : virtual public OmnRCObject
{
private:
	static AosVersionServerObjPtr		smVersionServer;

public:
	virtual bool    stop() = 0;
	virtual bool	addVersionObj(
					const AosXmlTagPtr &obj, 
					const AosRundataPtr &rdata) = 0;
	
	virtual u64		addDeletedVerObj(
					const AosXmlTagPtr &deldoc,
					const AosRundataPtr &rdata) = 0;

	inline static AosVersionServerObjPtr getVersionServer() {return smVersionServer;}
	static void setVersionServer(const AosVersionServerObjPtr &c) {smVersionServer = c;}
};
#endif
