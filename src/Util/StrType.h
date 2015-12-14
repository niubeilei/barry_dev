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
//	05/01/2007 Extracted from Random.h, Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_StrType_h
#define Omn_Util_StrType_h

#include "Util/String.h"


enum AosStrType
{
	eAosStrType_Invalid,
	eAosStrType_AsciiStr,
	eAosStrType_Printable,
	eAosStrType_NoSpaceStr,
	eAosStrType_BinStr, 
	eAosStrType_LowcaseStr,
	eAosStrType_CapitalStr,
	eAosStrType_DigitStr,
	eAosStrType_LetterStr,
	eAosStrType_LetterDigitStr,
	eAosStrType_VarName,
	eAosStrType_Symbol,
	eAosStrType_WhiteSpace,
	eAosStrType_WhiteSpaceWithNewLines,
	eAosStrType_NonPrintable,
	eAosStrType_ControlChar,
	eAosStrType_HHMMSS,
	eAosStrType_InvalidHHMMSS,

	eAosStrType_LastEntry
};

extern AosStrType AosStrType_strToEnum(const OmnString &name);
extern OmnString  AosStrType_enumToStr(const AosStrType type);
extern OmnString  AosStrType_getStrTypeNames();
#endif

