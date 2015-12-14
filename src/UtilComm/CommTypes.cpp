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
// 03/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/CommTypes.h"

#include "Util/String.h"


AosTcpLengthType AosConvertTcpLenType(const OmnString &str, 
		const AosTcpLengthType dft_value)
{
	if (str == "newline") return eAosTLT_TermByNewLine;
	if (str == "first_four_high") return eAosTLT_FirstFourHigh;
	if (str == "first_four_low") return eAosTLT_FirstFourLow;
	if (str == "first_word_high") return eAosTLT_FirstWordHigh;
	if (str == "first_word_low") return eAosTLT_FirstWordLow;
	if (str == "xml") return eAosTLT_Xml;
	if (str == "zero_term") return eAosTLT_TermByZero;
	if (str == "length_ind") return eAosTLT_LengthIndicator;
	if (str == "no_length_ind") return eAosTLT_NoLengthIndicator;
	return dft_value;
}


