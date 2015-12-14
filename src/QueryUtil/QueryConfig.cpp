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
// 01/02/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryUtil/QueryConfig.h"

#include "XmlUtil/XmlTag.h"

i64  AosQueryConfig::smQueryPageSize = AosQueryConfig::eDftQueryPageSize;
bool AosQueryConfig::smUsingSmartQuery = false;

bool 
AosQueryConfig::config(const AosXmlTagPtr &conf)
{
	if (!conf) return true;
	AosXmlTagPtr tag = conf->getFirstChild("query");
	if (!tag) return true;

	smQueryPageSize = tag->getAttrInt("bitmap_page_size", eDftQueryPageSize);
	smUsingSmartQuery = tag->getAttrBool("using_smart_query", false);

	return true;
}

