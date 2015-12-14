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
// 2014/12/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoEnum_h
#define Aos_JimoAPI_JimoEnum_h

#include <string>
using namespace std;

namespace Jimo
{
enum jimoQueryOperator
{
	GT,			// >
	GE,			// >= 
	EQ,			// =
	LT,			// <
	LE,			// <=
	NE			// !=
	AN,			// Match any
	PREFIX,		// Prefix match
	LIKE,		// Like
	DATE,		// date
	EPOCH,		// Epoch
	GE_LE,		// Range: (value1, value2)
	GE_LT,		// Range: (value1, value2]
	GT_LE,		// Range: [value1, value2)
	GT_Lt,		// Range: [value1, value2]
	IN,			// in
	DISTINCT,	// distinced
	PX			
	GT_LT
};
};

#endif

