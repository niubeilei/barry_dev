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
// 12/19/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ZikiApps_PollDbChanges_PollDbChanges_h
#define Aos_ZikiApps_PollDbChanges_PollDbChanges_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosPollDbChanges : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	enum
	{
		eFrequence = 500000,
		eNumTries = 50
	};

public:
	AosPollDbChanges();
	~AosPollDbChanges();

	int	process(const u32 snappoint, const OmnString &tname);

private:
};
#endif

