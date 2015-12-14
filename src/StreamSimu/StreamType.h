////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamType.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_StreamSimu_StreamType_h
#define Omn_StreamSimu_StreamType_h



class AosStreamType
{
public:
	enum E
	{
		eNormal  	 = 1,
		eWave    	 = 2,
		eBlock   	 = 3,
		eRandom  	 = 4,
		eFtp     	 = 5,	// Added by Chen Ding, 04/21/2007
	    eTelnet  	 = 6,	// Added by Chen Ding, 04/21/2007
		eSSH     	 = 7,	// Added by Chen Ding, 04/21/2007
		eWinTerminal = 8,	// Added by Chen Ding, 04/21/2007
	};
};
#endif

