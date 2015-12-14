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
// 11/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Action_ActionSetVar_h
#define Omn_Action_ActionSetVar_h

#include "Action/Action.h"
#include "Util/RCObjImp.h"

class AosActionSetVar : public AosAction 
{
	OmnDefineRCObject;

private:
	std::string		mVarName;
	std::string		mValueName;
   
public:	
	AosActionSetVar(const std::string &varame, const std::string &valuename);
	~AosActionSetVar();

	virtual int doAction(const AosVarContainerPtr &semanticData, 
						 const AosVarContainerPtr &actionData,
						 std::string &errmsg);
};
#endif

