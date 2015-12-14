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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogParser_LogParser_h
#define AOS_LogParser_LogParser_h

#include "SEInterfaces/LogParserObj.h"

class AosLogParser : public AosLogParserObj
{
protected:
	typedef hash_map<OmnString, AosLogParserObjPtr> 			map_t;
	typedef hash_map<OmnString, AosLogParserObjPtr>::iterator 	itr_t;

public:
	AosLogParser(const int version);
	AosLogParser();

	~AosLogParser();
};
#endif

