////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Capplet_CappletExample_h
#define AOS_Capplet_CappletExample_h

#include "Capplet/Capplet.h"

class AosCappletExample : public AosCapplet
{
	OmnDefineRCObject;

public:
	AosCappletExample();
	~AosCappletExample();

	virtual AosCappletPtr clone();

	virtual bool run(const AosRundataPtr &rdata, 
					AosDatalet &parms,
					const AosDataletPtr &datalet);
};
#endif

