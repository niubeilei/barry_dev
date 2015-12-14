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
// 05/14/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_Util_h
#define Omn_TorturerCond_Util_h

#include "Util/String.h"


enum AosTortCondOpr
{
	eAosTortCondOpr_Invalid,

	eAosTortCondOpr_Less,
	eAosTortCondOpr_LessEqual,
	eAosTortCondOpr_Equal,
	eAosTortCondOpr_LargerEqual,
	eAosTortCondOpr_Larger,
	eAosTortCondOpr_NotEqual,
};

extern AosTortCondOpr AosTorCondOpr_toEnum(const OmnString &opr);
extern int AosTimeStr_compare(const OmnString &v1, 
						 const OmnString &v2, 
						 OmnString &errmsg);
extern bool AosTimeStr_getValues(const OmnString &value, 
						 int &hh, 
						 int &mm, 
						 int &ss, 
						 OmnString &errmsg);

enum AosOperandType
{
	eAosOperandType_Invalid,

	eAosOperandType_Parm,
	eAosOperandType_Var,
	eAosOperandType_CrtValue,
	eAosOperandType_Constant
};

extern AosOperandType AosOperandType_toEnum(const OmnString &opr);
extern OmnString	  AosOperandType_toStr(const AosOperandType &type);
#endif

