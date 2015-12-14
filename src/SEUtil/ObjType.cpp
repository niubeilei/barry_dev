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
// 09/09/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/ObjType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/DocTags.h"


AosObjType::E 
AosObjType::convertToEnum(const OmnString &otype)
{
	OmnString tt = otype;
	if (tt == "") tt = AOSOTYPE_DEFAULT;

	const char *data = tt.data();
	switch (data[0])
	{
	case 'i':
		 if (tt == AOSOTYPE_IMAGE) return eImage;
		 break;

	case 'z':
		 if (tt.length() < 5 || data[1] != 'k' || data[2] != 'y' || data[3] != '_') 
		 {
			 return eDefault;
		 }

		 switch (data[4])
		 {
		 case 'a':
			  if (tt == AOSOTYPE_ACCESS_RCD) return eAccessRcd;
			  break;

		 case 'c':
			  if (tt == AOSOTYPE_COMMENT) return eComment;
			  if (tt == AOSOTYPE_CONTAINER) return eContainer;
			  break;

		 case 'g':
			  if (tt == AOSOTYPE_GROUP) return eUserGroup;
			  break;

		 case 'i':
			  if (tt == AOSOTYPE_IDGENMGR) return eIdGenMgr;
			  break;

		 case 'l':
			  if (tt == AOSOTYPE_LOGIN_LOG) return eLoginLog;
			  if (tt == AOSOTYPE_LOG) return eLog;
			  break;

		 case 'r':
			  if (tt == AOSOTYPE_ROLE) return eUserRole;
			  break;

		 case 's':
			  if (tt == AOSOTYPE_SESSION) return eSession;
			  if (tt == AOSOTYPE_SYSBACK_LOG) return eSysBackupLog;
			  break;

		 case 'u':
			  if (tt == AOSOTYPE_USERACCT) return eUserAccount;
			  break;

		 case 'v':
			  if (tt == AOSOTYPE_VERSION) return eVersion;
			  break;

		 default:
			  break;
		 }
		 break;

	default:
		 break;
	}

	return eDefault;
}

