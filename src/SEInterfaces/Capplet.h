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
// 2013/02/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_Capplet_h
#define Aos_SEInterfaces_Capplet_h

#include "SEInterfaces/Ptrs.h"


class AosCapplet : virtual public OmnRCObject
{
	static AosCappletPtr	smObject;

public:
	static void setCapplet(const AosCappletPtr &obj) {smObject = obj; }
	static AosCappletPtr getCapplet() {return smObject;}
};
#endif

