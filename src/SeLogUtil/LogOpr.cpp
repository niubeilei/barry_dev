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
// 09/18/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeLogUtil/LogOpr.h"


AosLogOpr::E 
AosLogOpr::toEnum(const OmnString &name)
{
	if (name.length() < 1) return eInvalid;
	const char *data = name.data();
	switch (data[0])
	{
	case 'a':
		 if (strcmp(data, AOSLOGOPR_ADD) == 0) return eAddLog;
		 if (strcmp(data, AOSLOGOPR_ADDVERSION) == 0) return eAddVersion;
		 break;

	case 'd':
		 if (strcmp(data, AOSLOGOPR_DELETE_VERSION) == 0) return eDeleteVersion;
		 break;

	case 'q':
		 if (strcmp(data, AOSLOGOPR_QUERY_LOG) == 0) return eQueryLogs;
		 break;

	case 'm':
		 if (strcmp(data, AOSLOGOPR_MARK_VERSION) == 0) return eMarkVersion;
		 break;

	case 'r':
		 if (strcmp(data, AOSLOGOPR_RETRIEVE) == 0) return eRetrieveLog;
		 if (strcmp(data, AOSLOGOPR_RETRIEVE_LOGIDS) == 0) return eRetrieveLogids;
		 if (strcmp(data, AOSLOGOPR_RETRIEVE_IILID) == 0) return eRetrieveIILId;
		 break;

	case 'v':
		 if (strcmp(data, AOSLOGOPR_RETRIEVE_VERSION) == 0) return eRetrieveVersion;
		 break;

	default:
		 break;
	}

	return eInvalid;
}

