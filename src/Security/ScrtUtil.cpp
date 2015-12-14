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
// 2009/10/19	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Security/ScrtUtil.h"

#include "SEUtil/Ptrs.h"
#include "Util/File.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"


extern bool AosCheckAccess(
		const int operation, 
		const AosXmlDocPtr &header,
		OmnString &errmsg, 
		AosXmlRc &errcode)
{
	return true;
}

