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
// 10/02/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/AcctType.h"

#include "Alarm/Alarm.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/ValueDefs.h"



AosAcctType::E 			
AosAcctType::toEnum(const OmnString &opr)
{
	if (opr == AOSVALUE_ACCTTYPE_TIMED) return eTimed;
	if (opr == AOSVALUE_ACCTTYPE_NUM) return eUsage;
	if (opr == AOSVALUE_ACCTTYPE_NORM) return eNormal;
	return eInvalid;
}


OmnString	
AosAcctType::toString(const E opr)
{
	switch (opr)
	{
	case eNormal:
		 return AOSVALUE_ACCTTYPE_NORM;

	case eTimed:
		 return AOSVALUE_ACCTTYPE_TIMED;

	case eUsage:
		 return AOSVALUE_ACCTTYPE_NUM;

	default:
		 OmnAlarm << "Unrecognized account type: " << opr << enderr;
		 return AOSVALUE_ACCTTYPE_NUM;
	}

	OmnShouldNeverComeHere;
	return AOSVALUE_ACCTTYPE_NUM;
}
	
