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
//
// Modification History:
// 11/24/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_RuleId_h
#define Aos_SemanticRules_RuleId_h

#include <string>

class AosRuleId
{
public:
	enum E
	{
		eInvalid, 
		eUnknown,

		eSR0001, 
		eSR00027, 
		eSR00028, 
		eSR00031, 
		eSR00033, 

		eMaxEntry
	};

	static std::string		toStr(const AosRuleId::E code);
	static AosRuleId::E		toEnum(const std::string &name);
};

#endif
