////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 12/05/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILUtil_IILTypes_h
#define AOS_IILUtil_IILTypes_h

#include "IILUtil/IILUtil.h"

enum
{
	eAosMaxSysAttrLen = 1000,
/*	eAosMaxObjidLen = 64,
	eAosMaxStypeLen = 32,
	eAosMaxOtypeLen = 32
*/
	// Ken Lee, 2013/06/07
	eAosMaxObjidLen = AosIILUtil::eMaxStrValueLen,
	eAosMaxStypeLen = AosIILUtil::eMaxStrValueLen,
	eAosMaxOtypeLen = AosIILUtil::eMaxStrValueLen
};

#endif
