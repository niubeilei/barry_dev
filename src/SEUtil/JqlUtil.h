//////////////////////////////////////////////////////////////////////////
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/08/03 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_JqlUtil_h
#define Aos_SEUtil_JqlUtil_h

#include "Util/String.h"
#include "Util/DataTypes.h"


class AosJqlUtil
{
public:
	static bool escape(OmnString &data)
	{
		data.replace(".", "0x2e", true);
		data.replace("<", "0x3c", true);
		data.replace(">", "0x3e", true);
		data.replace("(", "0x28", true);
		data.replace(")", "0x29", true);
		data.replace("/", "0x2f", true);
		data.replace("*", "0x2a", true);
		data.replace(",", "0x2c", true);
		data.replace("\"", "0x22", true);
		data.replace("'", "0x27", true);
		data.replace("-", "0x2d", true);
		data.replace(":", "0x3a", true);
		data.replace(" ", "0x20", true);
		//JIMODB-1011 Bryant 2015/10/22
		data.replace("=", "0x3d", true);
		data.replace("!", "0x21", true);
		data.replace("%", "0x25", true);
		return true;
	}

	static bool	unescape(OmnString &data)
	{
		data.replace("0x2e", ".", true);
		data.replace("0x3c", "<", true);
		data.replace("0x3e", ">", true);
		data.replace("0x27", "'", true);
		data.replace("0x28", "(", true);
		data.replace("0x29", ")", true);
		data.replace("0x2f", "/", true);
		data.replace("0x2a", "*", true);
		data.replace("0x2c", ",", true);
		data.replace("0x22", "\"", true);
		data.replace("0x20", " ", true);
		data.replace("0x2d", "-", true);
		data.replace("0x3a", ":", true);
		//JIMODB-1011 Bryant 2015/10/22
		data.replace("0x25", "%", true);
		data.replace("0x3d", "=", true);
		data.replace("0x21", "!", true);
		return true;
	}

	static OmnString stripFunc(OmnString &str)
	{
		int l_idx = str.indexOf(0, '(', false);
		if (l_idx < 0) return str;

		int r_idx = str.indexOf(0, ')', true);
		if (r_idx < 0) return str;

		if (l_idx + 1 > r_idx - 1) return str;

		OmnString substr = str.substr(l_idx + 1, r_idx - 1);
		return substr;
	}

	static bool isStr2StrMap(const AosDataType::E &type)
	{
		switch (type)
		{
		case AosDataType::eChar:
		case AosDataType::eString:
		case AosDataType::eShortString:
		case AosDataType::eLongString:
		case AosDataType::eCharStr:
		case AosDataType::eDateTime:
			return true;
		default:
			break;
		}
		return false;
	}

	static bool isStr2NumMap(const AosDataType::E &type)
	{
		switch (type)
		{
		case AosDataType::eU64:
		case AosDataType::eU32:
		case AosDataType::eU16:
		case AosDataType::eU8:
		case AosDataType::eInt64:
		case AosDataType::eInt32:
		case AosDataType::eInt16:
		case AosDataType::eInt8:
		case AosDataType::eBool:
		case AosDataType::eFloat:
		case AosDataType::eDouble:
		case AosDataType::eNumber:
			return true;
		default:
			break;
		}

		return false;
	}	
};

#endif

