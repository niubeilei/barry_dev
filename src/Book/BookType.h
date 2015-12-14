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
// 	Created: 12/14/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_BookType_h
#define Omn_Book_BookType_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <string.h>

class AosBookType
{
public:
	enum E
	{
		eUnknown,

		eWebbook = 'W',
		eSpreadsheet = 'S',

		eMaximum
	};

	static std::string toStr(AosBookType::E type);
	static AosBookType::E toEnum(const char *name);
};
#endif
