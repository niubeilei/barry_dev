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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtConfig.h"


u32 AosJqlStmtConfig::smPageSize = 100;


bool
AosJqlStmtConfig::config(const AosXmlTagPtr &conf)
{ 
	AosXmlTagPtr tag = conf->getFirstChild("jql_statements");
	if (!tag) return true;

	smPageSize = tag->getAttrU32("page_size", smPageSize);
	if (smPageSize <= 0)
	{
		smPageSize = 100;
	}

	return true;
}


