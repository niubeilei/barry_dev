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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SysDefValObj_h
#define Aos_SEInterfaces_SysDefValObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"


class AosSysDefValObj : virtual public AosJimo
{
protected:
	static AosSysDefValObjPtr	smObject;

public:
	AosSysDefValObj(const int version);
	~AosSysDefValObj();

	static void setSysDefValObj(const AosSysDefValObjPtr &obj) {smObject = obj;}

	static bool rsolveStatic(
						const AosRundataPtr &rdata, 
						const OmnString &val_name, 
						const OmnString &dft, 
						OmnString &value);

	virtual bool resolve(const AosRundataPtr &rdata, 
						const OmnString &val_name, 
						const OmnString &dft, 
						OmnString &value) = 0;
};
#endif

