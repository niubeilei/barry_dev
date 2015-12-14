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
// 2014/04/21 Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStmtConfig_JQLSTATEMENT_H
#define AOS_JQLStmtConfig_JQLSTATEMENT_H

#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosJqlStmtConfig
{
private:
	static u32				smPageSize;

public:
	static bool config(const AosXmlTagPtr &conf);
	static u32 getPageSize() {return smPageSize;}
};
#endif
