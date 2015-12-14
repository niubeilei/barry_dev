////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MinMax.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_MinMax_h
#define Omn_Porting_MinMax_h



#ifdef OMN_PLATFORM_UNIX
//
// It is Unix platform
//
class OmnMinMax
{
public:
	enum
	{
		eUint4Max = 0xffffffff
	};
};






#elif OMN_PLATFORM_MICROSOFT
class OmnMinMax
{
public:
	enum
	{
		eUint4Max = 0xffffffff
	};
};

#endif
#endif
