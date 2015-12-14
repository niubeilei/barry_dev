////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ModuleId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_ModuleId_h
#define Omn_Message_ModuleId_h

#include "Util/String.h"

class OmnModuleId
{
public:
	enum E
	{
		eFirstValidEntry = 0,
		
		eInvalid,

		eRealmMgr,
		eRealmImage,

		eLastValidEntry
	};

	static bool	isValid(const E c)
	{
		return (c > eFirstValidEntry && c < eLastValidEntry);
	}

	static OmnString toStr(const E c)
	{
		switch (c)
		{
		case eRealmMgr:
			 return "RealmMgr";

		case eRealmImage:
			 return "RealmImage";

		case eFirstValidEntry:
		case eLastValidEntry:
			 return "OnBoundary";

		case eInvalid:
			 return "Invalid";

		default:
			 return "NotDefined";
		}

		return "Incorrect";
	}
};
#endif
