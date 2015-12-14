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
// Generic Table serves as a database table. It contains a list
// of records, each of which is expressed by instances  of AosGenRecord. 
// Each record is identified through names. 
//
// Modification History:
// 07/20/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Util/GenTableUtil.h"

#include "Alarm/Alarm.h"


OmnString AosGenFieldType_toStr(const AosGenFieldType type)
{
	switch (type)
	{
	case eAosGenFieldType_Invalid:
		 return "Invalid";

	case eAosGenFieldType_Int: 
		 return "Integer";

	case eAosGenFieldType_U32:
		 return "U32";

	case eAosGenFieldType_Str:
		 return "String";

	case eAosGenFieldType_Table:
		 return "Table";

	default:
		 OmnAlarm << "Unrecognized type: " << type << enderr;
		 return "Unknown";
	}

	return "Error";
}




