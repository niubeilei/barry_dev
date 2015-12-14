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
// Created: 08/04/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Book/TagMgr.h"

#include "Book/Ptrs.h"
#include "Book/Tag.h"
#include "util_c/strutil.h"

AosTagMgr::AosTagMgr() 
{
}


bool
AosTagMgr::procTags(
		const OmnString &tags, 
		const AosSystemData &data)
{
	// A data 'dataid' has been created and the data is associated
	// with the tags 'tags'. This function:
	// 1. Adds an entry for each tag into 'tag_table'.
	// 2. For each tag, check whether there are containers that 
	//    registered for the tag. 

	// 1. Adds entries to tag_table. Tags are represented as:
	// 	"tag1|$|tag2|$|...
	char *parts[eMaxTags];
	int num = aos_sstr_split(tags.data(), "|$|", parts, eMaxTags);
	if (num > 0)
	{
		AosTag tt;
		tt.addEntries(parts, num, data);
		aos_str_split_releasemem(parts, num);
	}

	// 2. For each tag, check the registered. If a table registered 
	// a tag, that means when an entry with that tag is added to 
	// 'tag_table', that entry should be added to that table, too.
	return true;
}
