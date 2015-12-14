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
//
// Modification History:
// 02/26/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Security_SecReqDriver_h
#define Omn_Security_SecReqDriver_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/SeqReqObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSecReqDriver : public AosSeqReqObj 
{
	OmnDefineRCObject;

public:
	virtual OmnString		getRequesterDomains() const;
	virtual OmnString		getRequesterUserRoles() const;
	virtual OmnString		getRequesterUserGroups() const;
};
#endif

