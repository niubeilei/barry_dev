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
#ifndef Aos_Capplet_CappletImp_h
#define Aos_Capplet_CappletImp_h

#include "SEInterfaces/Capplet.h"

class AosCappletImp : public AosCapplet
{
protected:

public:
	AosCappletImp(const AosCappletId::E type);
	virtual ~AosCappletImp();

	virtual AosCappletPtr clone() = 0;

	virtual bool run(const AosRundataPtr &rdata, 
					AosDatalet &parms,
					const AosDataletPtr &datalet) = 0;
};
#endif
