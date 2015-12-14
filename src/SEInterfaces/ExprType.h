////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2013/12/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ExprType_h
#define Aos_SEInterfaces_ExprType_h


class AosExprType
{
public:
	enum E
	{
		eInvalid, 

		eArith,
		eAtomic,
		eBitArith,
		eBinary,
		eBrackets,
		eComparison,
		eContain,
		eDouble,
		eFieldName,
		eFunctions,
		eGenFunc,
		eGetDocid,
		eLogic,
		eMemberOpt,
		eNumber,
		eString,
		eLike,
		eNameValue,
		eIsNotNull,
		eIsNull,
		eIn,
		eNotIn,
		eNull,
		eCase,
		eUserVar,

		eMaxExprTypeEntry
	};
};
#endif

