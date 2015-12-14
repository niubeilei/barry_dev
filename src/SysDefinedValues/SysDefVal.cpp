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
#include "SysDefinedValues/SysDefVal.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Mutex.h"


AosSysDefVal::AosSysDefVal(
		const OmnString &type,
		const int version)
:
AosSysDefValObj(version),
mType(type)
{
}


AosSysDefVal::~AosSysDefVal()
{
}


bool
AosSysDefVal::resolveWithMember(
		const AosRundataPtr &rdata, 
		const OmnString &val_type, 
		const OmnString &val_member, 
		const OmnString &dft, 
		OmnString &value)
{
	OmnShouldNeverComeHere;
	return 0;
}

