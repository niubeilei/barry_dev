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
//  Moved from TransUtil by Chen Ding, 2015/03/31
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_UtilData_ModuleId
#define AOS_UtilData_ModuleId

#include "Alarm/Alarm.h"
#include "XmlUtil/DocTypes.h"

#define AOSMODULEID_INVALID			"invalid"

#define AOSMODULEID_DOC				"doc"
#define AOSMODULEID_IIL				"iil"
#define AOSMODULEID_JOBMGR			"jobmgr"
#define AOSMODULEID_TASKMGR			"taskmgr"


class AosModuleId : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
	{
		eInvalid               = 0,

		eDoc				   = 1,
		eIIL				   = 2,
		eLog				   = 3,
		eCounter			   = 4,
		eInstantMessage		   = 5,
		eShortMessage		   = 6,
		eEmail				   = 7,
		eTaskMgr			   = 8,
		eJobMgr				   = 9,
		eSQL				   = 10,
		eReplicMgr			   = 11,
		eServerMgr			   = 12,
		
		eQueryEngine		   = 13,	// Chen Ding, 2013/01/02
		eQueryCache			   = 14,	// Ken Lee, 2013/03/26

		eBitmap				   = 15,	// Chen Ding, 2013/01/09
		eBinaryDoc			   = 16,	// Chen Ding, 2013/01/10
		eAccessRcd			   = 17,	// Ic Yu, 2013/01/19
		eBitmapTree			   = 18,	// Chen Ding, 2013/02/05
		eBitmapCache		   = 19,	// Chen Ding, 2013/02/05
		eBitmapEngine		   = 20,	// Chen Ding, 2013/02/06
		eCubeMsgRouter		   = 21,

		eLocalBitmapStore	   = 22,
		eBitmapBlockIndex	   = 23,
		eSectionedBitmap	   = 24,
		eStatistics			   = 25,	// Chen Ding, 2015/01/04
		eConfigMgr			   = 26,	// White, 2015/10/21
		eGroupDoc			   = 27,	// Gavin, 2015/10/23

		eMax
	};
protected:
	E 	mModuleId;

	AosModuleId(const E id);
	~AosModuleId();

public:
	static bool	isValid(const E code)
	{ 
		return code > eInvalid && code < eMax;
	}

	static bool init();
	static E toEnum(const OmnString &name);
	static OmnString toStr(const E code);
	static bool addName(const OmnString &name, const E code);

	// Xuqi, 2015/10/26
	static E docType2ModuleID(const AosDocType::E doc_type)
	{
		switch (doc_type)
		{
		case AosDocType::eAccessDoc: return eAccessRcd;
		case AosDocType::eNormalDoc: return eDoc;
		case AosDocType::eIIL: return eIIL;
		default: break;
		}

		OmnShouldNeverComeHere;
		return eInvalid;
	}
};
#endif

