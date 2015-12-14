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
// 2013/06/15 Recreated by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Logger/Logger.h"

#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosLogger::AosLogger()
{
}


AosLogger::~AosLogger()
{
}


bool	
AosLogger::start()
{
}


bool 	
AosLogger::stop()
{
}


bool	
AosLogger::config(const AosXmlTagPtr &def)
{
}


bool 
AosLogger::createLogger(	
		const AosRundataPtr &rdata, 
	 	const OmnString &log_name)
{
}


bool 
AosLogger::removeLogger(	
		const AosRundataPtr &rdata, 
	 	const OmnString &log_name)
{
}


bool 
AosLogger::addEntry(	
		const AosRundataPtr &rdata, 
	 	const OmnString &log_name,
	 	const OmnString &entry)
{
}


bool 
AosLogger::retrieveEntry(
		const AosRundataPtr &rdata, 
		const OmnString &log_name,
	 	const u64 entry_id, 
		OmnString &entry,
		const AosLogCallerPtr &caller)
{
}


bool 
AosLogger::retrieveEntries(
		const AosRundataPtr &rdata, 
		const OmnString &log_name,
	 	const vector<u64> &entry_id, 
		vector<OmnString> &entries,
		const AosLogCallerPtr &caller)
{
}
#endif
