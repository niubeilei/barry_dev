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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/FieldName.h"


OmnString 
AosFieldName::toStr(const E code)
{
	switch (code)
	{
	case eInvalidFieldName:	return "no_name";
	case eErrorType:		return "error_type";
	case eDocid:			return "Docid";
	case eSnapshot:			return "Snapshot";
	case eUserid:			return "Userid";
	default:
		 break;
	}

	OmnString name = "field_";
	name << code;
	return name;
}

