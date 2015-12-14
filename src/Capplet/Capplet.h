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
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Capplet_Capplet_h
#define Aos_Capplet_Capplet_h

#include "Capplet/CappletId.h"
#include "Capplet/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosCapplet : virtual public OmnRCObject
{
protected:

public:
	AosCapplet(const AosCappletId::E type);
	virtual ~AosCapplet();

	virtual AosCappletPtr clone() = 0;

	virtual bool run(const AosRundataPtr &rdata, 
					AosDatalet &parms,
					const AosDataletPtr &datalet) = 0;
};
#endif
