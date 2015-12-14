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
// 12/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef OMN_Accesses_AccessUserEnum_h
#define OMN_Accesses_AccessUserEnum_h

#include "Rundata/Ptrs.h"
#include "Accesses/Access.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosAccessUserEnum : public AosAccess
{
	OmnDefineRCObject;

public:
	AosAccessUserEnum();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool checkAccess(
					const AosRundataPtr &rdata,
					AosSecReq &sec_req, 
					const AosXmlTagPtr &tag, 
					bool &granted,
					bool &denied);
};
#endif

