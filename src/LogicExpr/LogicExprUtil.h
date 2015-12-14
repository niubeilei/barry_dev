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
// 07/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_LogicExpr_LogicExprUtil_h
#define Omn_LogicExpr_LogicExprUtil_h

#include "TorturerConds/Util.h"
#include "Util/String.h"


enum AosDataType
{
	eAosDataType_Invalid,

	eAosDataType_Integral,
	eAosDataType_Unsigned,
	eAosDataType_String
};

extern AosDataType 	AosDataType_toEnum(const OmnString &name);
extern bool 		AosEvaluate(const AosDataType dataType, 
						const OmnString &value1, 
						const OmnString &value2, 
						const AosTortCondOpr opr,
						bool &rslt, 
						OmnString &errmsg);
extern bool 		AosEvaluateInt(
						const OmnString &value1, 
						const OmnString &value2, 
						const AosTortCondOpr opr,
						bool &rslt, 
						OmnString &errmsg);
extern bool 		AosEvaluateUnsigned(
						const OmnString &value1, 
						const OmnString &value2, 
						const AosTortCondOpr opr,
						bool &rslt, 
						OmnString &errmsg);
extern bool 		AosEvaluateString(
						const OmnString &value1, 
						const OmnString &value2, 
						const AosTortCondOpr opr,
						bool &rslt, 
						OmnString &errmsg);

#endif

