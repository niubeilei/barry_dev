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
// 09/28/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocTypes_h
#define Aos_SmartDoc_SdocTypes_h

#include "Util/String.h" 

#define AOSSDOCOPR_QUERYONLY        "query"
#define AOSSDOCOPR_SIMULATE         "simulate"
#define AOSSDOCOPR_RUN              "run"

class AosSdocOpr
{
public:
	enum E
	{
		eInvalid,

		eQueryOnly,
		eSimulate,
		eRun,

		eMax
	};

	OmnString toStr(const E code)
	{
		switch (code)
		{
		case eQueryOnly:
			 return AOSSDOCOPR_QUERYONLY;

		case eSimulate:
			 return AOSSDOCOPR_SIMULATE;

		case eRun:
			 return AOSSDOCOPR_RUN;
			 
		default:
			 return "invalid";
		}
		return "invalid";
	}

	static	E toEnum(const OmnString &name)
	{
		if (name == AOSSDOCOPR_QUERYONLY) 	return eQueryOnly;
		if (name == AOSSDOCOPR_SIMULATE) 	return eSimulate;
		if (name == AOSSDOCOPR_RUN) 		return eRun;
		return eInvalid;
	}
};
#endif

