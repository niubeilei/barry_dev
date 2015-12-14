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
// Created: 08/04/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_DataType_h
#define Omn_Book_DataType_h

#include "Util/String.h"


class AosSystemDataType
{
public:
	enum E
	{
		eInvalid,

		eImage, 
		ePicture,
		eTable,
		eRecord,

		eMax
	};

	static OmnString getStr(const AosSystemDataType::E code);
	static E getCode(const OmnString &name);
};

#endif

