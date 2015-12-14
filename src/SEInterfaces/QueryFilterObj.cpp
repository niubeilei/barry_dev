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
// 2014/08/26	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryFilterObj.h"


OmnString
AosQueryFilterObj::EnumToStr(const FilterType type)
{
	switch (type)
	{
	case eKeyField : return "keyfield";
	case eStrKey : return "strkey";
	case eU64Key : return "u64key";
	case eValue : return "value";
	default : break;
	}
	return "invalid";
}


AosQueryFilterObj::FilterType
AosQueryFilterObj::StrToEnum(const OmnString &str)
{
	switch (str.data()[0])
	{
	case 'k' :
		 if (str == "keyfield") return eKeyField;
		 break;
	case 's' : 
		 if (str == "strkey") return eStrKey;
		 break;
	case 'u' : 
		 if (str == "u64key") return eU64Key;
		 break;
	case 'v' :
		 if (str == "value") return eValue; 
		 break;
	default :
		 break;	
	}
	return eInvalid;
}

