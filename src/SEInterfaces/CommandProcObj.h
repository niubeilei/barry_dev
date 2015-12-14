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
// 2013/08/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CommandProcObj_h
#define Aos_SEInterfaces_CommandProcObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosCommandProcObj : virtual public AosJimo
{
public:
	virtual bool run(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &command) = 0; 
};
#endif
