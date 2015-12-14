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
//
// Modification History:
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SqlUtil_SqlUtil
#define Omn_SqlUtil_SqlUtil

#include "SqlUtil/TableNames.h"
#include "Util/String.h"

class AosSqlUtil
{
public:
	static OmnString composeStrValueTablename(const OmnString &container, const u32 siteid)
	{
		OmnString tname = AOSTNAME_PREF_STRVALUE;
		tname << "_" << container << "_" << siteid;
		return tname;
	}

	static OmnString composeInt64ValueTablename(const OmnString &container, const u32 siteid)
	{
		OmnString tname = AOSTNAME_PREF_INT64VALUE;
		tname << "_" << container << "_" << siteid;
		return tname;
	}

	static OmnString composeU64ValueTablename(const OmnString &container, const u32 siteid)
	{
		OmnString tname = AOSTNAME_PREF_U64VALUE;
		tname << "_" << container << "_" << siteid;
		return tname;
	}
};
#endif

