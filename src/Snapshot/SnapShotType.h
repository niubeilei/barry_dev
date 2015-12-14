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
#ifndef Aos_Snapshot_SnapShotType_h
#define Aos_Snapshot_SnapShotType_h

#include "Snapshot/SnapShotType.h"
#include "Thread/Mutex.h"
#include "Util/String.h" 
#include "Util/HashUtil.h"

#define AOSSNAPSHOT_IILTRANS				"iiltrans"
#define AOSSNAPSHOT_DFM						"dfm"

struct AosSnapShotType 
{
public:
	enum E
	{
		eInvalid, 
		eDfmSnapShot = 1,
		eIILTransSnapShot = 2,

		eMax
	};

	static bool isValid(const E type)
	{
		 return (type > eInvalid && type < eMax);
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() <= 0) return eInvalid;

		const char *data = name.data();
		switch(data[0])
		{
		case 'd':
			return eDfmSnapShot;

		case 'i':
			return eIILTransSnapShot;

		default:
			break;
		}
		OmnAlarm << "Invalid type: " << name << enderr;
		return eInvalid;
	}

	static OmnString toString(const E type)
	{
		switch(type)
		{
		case eDfmSnapShot:
			return "dfm";

		case eIILTransSnapShot:
			return "iiltrans";

		default:
			break;
		}
		OmnAlarm << "Invalid type: " << type << enderr;
		return "";
	}
};

#endif

