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
// 06/28/2012 Created by Brian
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_FullHandlerType_h
#define Aos_DataBlob_FullHandlerType_h


#define 		AOSFULLHANDLER_ACTIONS				"actions"

class AosFullHandlerType 
{
public:
	enum E
	{
		eInvalid,

		eActions,

		eMax
	};

	inline static	E toEnum(const OmnString &typestr)
	{
		if (typestr == AOSFULLHANDLER_ACTIONS) return eActions;
		else
		{
			OmnAlarm << "Invalid type" << enderr;
			return eInvalid;
		}

		return eInvalid;
	}
};
#endif
