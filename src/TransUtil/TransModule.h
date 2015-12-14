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
// 	Created: 07/07/2011 by Ice Yu
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TransModule
#define AOS_TransUtil_TransModule

#include "Alarm/Alarm.h"

class AosTransModule : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum ModuleId
	{                                                             
		eInvalid                            = 0,

		eDoc,
		eIIL,
		eLog,
		eCounter,
		eInstantMessage,
		eShortMessage,
		eEmail,
		eTaskMgr,
		eJobMgr,
		eSQL,
		eSIIL,						// For big IIL
		eStorageMgr,
		eNetFileServer,
		eQueryEngine,				// Chen Ding, 2013/01/02
		eBitmap,					// Chen Ding, 2013/01/09
		eBinaryDoc,					// Chen Ding, 2013/01/10
		eAccessRcd,					// Ic Yu, 2013/01/19

		eMax
	};
protected:
	ModuleId 	mModuleId;

	AosTransModule(const ModuleId id)
	:
	mModuleId(id)
	{
	}

	~AosTransModule()
	{
	}
};
#endif

