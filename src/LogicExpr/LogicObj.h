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
//  This class defines a super class for all "Logic Objects". A Logic Object
//  is used by a logic expression to retrieve some values when it evaluates
//  its values. 
//
// Modification History:
// 07/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_LogicExpr_LogicObj_h
#define Omn_LogicExpr_LogicObj_h

#include "LogicExpr/Ptrs.h"
#include "LogicExpr/LogicExprUtil.h"
#include "TorturerConds/Util.h"
#include "Util/RCObject.h"
#include "XmlParser/Ptrs.h"


class AosLogicObj : virtual public OmnRCObject
{
public:
	virtual OmnString	toString() const {return "LogicObj";}
	virtual bool		getValue(const AosOperandType type, 
							 const OmnString &name, 
							 OmnString &value, 
							 OmnString &errmsg) {return false;}
};

#endif

