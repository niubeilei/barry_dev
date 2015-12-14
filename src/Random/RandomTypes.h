////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.h
// Description:
//  This file contains a group of random functions and a random class. 
//  The OmnRandom class is obselete. You are encouraged to use the 
//  random functions.  
//
// Modification History:
// 07/07/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomTypes_h
#define Omn_Random_RandomTypes_h

#include "Util/String.h"

enum AosTimeStrFormat_e
{
	eAosTimeStrFormat_Invalid, 

	eAosTimeStrFormat_HH, 
	eAosTimeStrFormat_HHMM, 
	eAosTimeStrFormat_HHMMSS, 
};

class AosRandType
{
public:
	enum E
	{
		eInvalid,

		eCellPhoneNumber,
		eDate,
		eDigitString,
		eNormal,

		eMax
	};

	static E	toEnum(const OmnString &type)
	{
		if (type == "cellphone") return eCellPhoneNumber;
		else if (type == "date") return eDate;
		else if (type == "digitstring") return eDigitString;
		else if (type == "normal") return eNormal;
		else return eInvalid;
	}

	static bool	isValid(const E type)
	{
		return type > eInvalid && type < eMax;
	}

	static OmnString toString(const E type)
	{
		switch(type)
		{
		case eCellPhoneNumber : return "cellphone";
		case eDate : return "date";
		case eDigitString : return "digitstring";
		case eNormal : return "normal";
		default : return "";
		}
		return "";
	}
};


extern AosTimeStrFormat_e AosGetTimeStrEnum(const OmnString &name);
#endif

