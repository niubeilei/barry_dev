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
// This is a utility class.  
//
// Modification History:
// 06/15/211: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_ResolveCounterName_h
#define Aos_CounterUtil_ResolveCounterName_h

#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "SEUtil/DocTags.h"
#include "Rundata/Rundata.h"
#include "Util/StrSplit.h"

class AosResolveCounterName
{
private:

public:
	AosResolveCounterName();
	~AosResolveCounterName(){}
	static bool resolveCounterName(
			const AosXmlTagPtr &def, 
			const OmnString &tagname,
			OmnString &resolvedName,
			const bool need_proc_member, 
			const AosRundataPtr &rdata);

	static bool resolveCounterName(
			const AosXmlTagPtr &cNameTmp,
			OmnString &resolvedName,
			const bool need_proc_member, 
			const AosRundataPtr &rdata);

	static bool resolveCounterName2(
			const AosXmlTagPtr &def,
			const OmnString &tagname,
			OmnString &resolvedName,
			i64 &level,
			const AosRundataPtr &rdata);

	static bool resolveCounterName2(
			const AosXmlTagPtr &def,
			const OmnString &tagname,
			OmnString &resolvedName,
			const AosRundataPtr &rdata);

	static bool	resolveCounterName2(
			const AosXmlTagPtr &cNameTmp,
			OmnString &resolvedName,
			const AosRundataPtr &rdata);

	static bool resolveCounterName3(
			const AosXmlTagPtr &def,
			const OmnString &tagname,
			OmnString &resolvedName1,
			OmnString &resolvedName2,
			i64 &level,
			const AosRundataPtr &rdata);

	static bool	resolveCounterName3(
			const AosXmlTagPtr &cNameTmp,
			OmnString &resolvedName1,
			OmnString &resolvedName2,
			const AosRundataPtr &rdata);
};
#endif

