////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 06/17/2011	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Timer_TimerTypes_h
#define AOS_Timer_TimerTypes_h

#define AOSTIMERTYPE_XML  		'x'
#define AOSTIMERTYPE_MEMORY		'm'

enum AosTimerType
{
	eInvalid, 
		
	eXmlTimer,
	eMemoryTimer,
		
	eMax
};


class AosTimerTypes
{

public:
	
	static AosTimerType toEnum(const char name)
	{
		switch (name)
		{
		case AOSTIMERTYPE_XML: 		return eXmlTimer;
		case AOSTIMERTYPE_MEMORY:	return eMemoryTimer;
		default: 					return eInvalid;
		}
	}

	static char toChar(AosTimerType type)
	{
		switch(type)
		{
		case eXmlTimer : return AOSTIMERTYPE_XML;
		case eMemoryTimer : return AOSTIMERTYPE_MEMORY;
		default : return AOSTIMERTYPE_XML;
		}
	}
	
	static bool isValid(const AosTimerType c) {return c > eInvalid && c < eMax;}	
};

#endif
