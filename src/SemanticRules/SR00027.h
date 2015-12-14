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
// For the definition of SR00027, please refer to the document.  
//
// Modification History:
// 12/04/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_SR00027_h
#define Aos_SemanticRules_SR00027_h

#include "aosUtil/Types.h"
#include "SemanticRules/SemanticRuleCommon.h"
#include "SemanticData/Types.h"
#include "Util/RCObjImp.h"
#include <string>
#include <list>

class AosSR00027 : public AosSemanticRuleCommon
{
	OmnDefineRCObject;

private:
	std::string				mFuncName;

public:
	AosSR00027(const std::string &funcName);
	~AosSR00027();

	virtual int	evaluate(const AosVarContainerPtr &data, 
						 std::string &errmsg);
	virtual bool procEvent(const AosEventPtr &event, 
						 bool &cont);
};
#endif

