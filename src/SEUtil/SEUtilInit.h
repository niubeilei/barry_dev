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
// 01/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_SEUtilInit_h
#define AOS_SEUtil_SEUtilInit_h

#include "XmlUtil/Ptrs.h"


class AosSEUtilInit
{
public:
	static bool init(const AosXmlTagPtr &conf);
};
#endif
