////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommTypes.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NativeAPI_UtilComm_CommTypes_h
#define Aos_NativeAPI_UtilComm_CommTypes_h


class OmnString;

class OmnCommTypes
{
public:
	enum 
	{
		eMinStreamPort    = 10000,
		eMaxStreamPort    = 60000,
		eMaxUdpPacketSize = 50000,
		eMaxTcpPacketSize = 50000,
		eReconnectTimer	  = 2,		// 2 seconds
		eReadFailIntervalTimerSec = 5,
		eReadingTimer     = 30,
		eMaxSockToSelect  = 10000
	};
};

enum AosTcpLengthType
{
	eAosTLT_InvalidLenType,
	eAosTLT_NoLengthIndicator,

	eAosTLT_FirstFourHigh,
	eAosTLT_FirstFourLow,
	eAosTLT_FirstWordHigh,
	eAosTLT_FirstWordLow,
	eAosTLT_TermByNewLine,
	eAosTLT_TermByZero,
	eAosTLT_Xml,
	eAosTLT_LengthIndicator
};

extern AosTcpLengthType	AosConvertTcpLenType(const OmnString &len, 
						const AosTcpLengthType dft_value = eAosTLT_FirstFourHigh);

#endif

