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
// 2013/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_Filenames_h
#define AOS_SEUtil_Filenames_h

#include "API/AosApi.h"
#include "Util/String.h"

#define AOSFILENAME_BITMAP_CACHE_ID			"bitmap_cache_id"

#define AOS_DIRNAME_SECTION_CACHE			"section_cache"

struct AosFilename
{
	static OmnString getBitmapCacheIdFilename()
	{
		return AOSFILENAME_BITMAP_CACHE_ID;
	}

	static OmnString getSectionCacheDirname(const bool flag)
	{
		OmnString dirname = AosGetWorkingDirname();
		dirname << "/" << AOS_DIRNAME_SECTION_CACHE;
		if (!flag) return dirname;

		OmnString command = "mkdir -p ";
		command << dirname;
		system(command.data());
		return dirname;
	}
};
#endif
