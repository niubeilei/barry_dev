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
#include "SEUtil/SEUtilInit.h"

#include "Util/Separators.h"
#include "XmlUtil/XmlTag.h"

bool 
AosSEUtilInit::init(const AosXmlTagPtr &conf)
{
	bool rslt = AosSeparators::config(conf);
	aos_assert_r(rslt, false);
	return true;
}
