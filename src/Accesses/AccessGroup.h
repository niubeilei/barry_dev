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
// 10/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef OMN_Accesses_AccessGroup_h
#define OMN_Accesses_AccessGroup_h

#include "Rundata/Ptrs.h"
#include "Accesses/Access.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosAccessGroup : public AosAccess
{
	OmnDefineRCObject;

public:
	AosAccessGroup();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool checkAccess(
					const AosRundataPtr &rdata,
					AosSecReq &sec_req, 
					const AosXmlTagPtr &tag, 
					bool &granted,
					bool &denied);
};
#endif

