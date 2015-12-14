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
// 08/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEInterfaces_VirtualFileCreatorObj_h
#define Aos_SEInterfaces_VirtualFileCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosVirtualFileCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerVirtualFile(
			const OmnString &name, 
			const AosVirtualFileObjPtr &file) = 0;

	virtual AosVirtualFileObjPtr createVirtualFile(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;
};
#endif
#endif
