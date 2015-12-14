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
// 06/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_AllStorageEngines_h
#define Aos_StorageEngine_AllStorageEngines_h


#include "XmlUtil/Ptrs.h"

struct AosAllStorageEngines
{
	static bool init(const AosXmlTagPtr &config);
};
#endif

