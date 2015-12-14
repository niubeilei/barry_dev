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
// The super class for all semantics data. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticData/Types.h"

#include "Alarm/Alarm.h"


std::string
AosTimeMeasure::getErrMsg(const u32 min, 
						  const u32 max, 
						  const AosTimeMeasure::E measure)
{
	std::string errmsg;
	switch (measure)
	{
	case eOffsetOfCrtTime:
		 // Not implemented yet;
		 return "not implemented yet";

	case eLastInstCreated:
		 return "since last instance created";

	case eLastInstDeleted:
		 return "since last instance deleted";

	case eLastInstRead:
		 return "since last instance read";

	case eLastInstModified:
		 return "since last instance modified";

	case eLastReset:
		 return "since last reset";

	case eLastMarker:
		 return "since last marker modified";

	case eLastCounter:
		 return "since last counter modified";

	case eLastPat:
		 return "since last patter being pat";

	default:
		 OmnAlarm << "Unrecognized time measure: " 
			 << measure << enderr;
		 errmsg = "Unrecognized time measure: ";
		 break;
	};

	return "This is an error";
}


