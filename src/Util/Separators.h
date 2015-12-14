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
// 01/01/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_Separators_h
#define AOS_Util_Separators_h

#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosSeparators
{
public:
	enum
	{
		eMaxLevels = 8
	};

	static char smSeparators[eMaxLevels];

public:
	static bool config(const AosXmlTagPtr &conf);

	static char getSeparator(const int level);
	static bool convert(const OmnString &str, char &value);
};
#endif
