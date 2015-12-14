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
// 2015/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicSchema_h
#define AOS_JimoLogicNew_JimoLogicSchema_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicSchema : virtual public AosJimoLogicNew
{
protected:
	OmnString						mCharset;
	OmnString						mSchemaName;

public:
	virtual ~AosJimoLogicSchema(){};

};
#endif

