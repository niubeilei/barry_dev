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
// 07/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Statemachine_Transit_h
#define Aos_Statemachine_Transit_h

#include "Rundata/Ptrs.h"
#include "Statemachine/Ptrs.h"
#include "Statemachine/TransitId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosTransit : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

protected:
	OmnString			mTransitName;
	AosTransitId::E		mTransitType;	

public:
	AosTransit(const OmnString &name, const AosTransitId::E type, const bool regflag);
	~AosTransit();

	virtual AosTransitPtr 	clone() = 0;
	virtual bool  run(const AosRundataPtr &rdata) = 0;

	AosTransitPtr getTransit(const OmnString &transit_id);

private:
	bool 	registerTransit(const AosTransitPtr &transit);
};
#endif

