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
#ifndef Aos_SysDefinedValues_SysDefVal_h
#define Aos_SysDefinedValues_SysDefVal_h

#include "SEInterfaces/SysDefValObj.h"
#include "Util/String.h"


class AosSysDefVal : public AosSysDefValObj
{
protected:
	OmnString		mType;

public:
	AosSysDefVal(const OmnString &type, const int version);
	~AosSysDefVal();
	
	virtual bool resolveWithMember(
						const AosRundataPtr &rdata, 
						const OmnString &val_type,
						const OmnString &val_member, 
						const OmnString &dft, 
						OmnString &value);
};
#endif



