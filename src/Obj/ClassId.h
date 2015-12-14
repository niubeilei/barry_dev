////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ClassId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Obj_ClassId_h
#define Snt_Obj_ClassId_h

#include <string.h>


#ifndef OmnDefineClass
#define OmnDefineClass(x, y) OmnClassId::defineClassId(x, y)
#endif

struct OmnClassId
{
public:
	enum
	{
		eMaxClassIdLength = 30
	};

	enum E
	{
		eFirstValidEntry,

		eOmnInvalid,
		eOmnUnknown,

		eOmnAlarmEntry,
		eOmnAlarmLogger,
		eOmnAlarmMgr,
		eOmnAlarmSender,
		eOmnAppInfo,

		eOmnCheckedout,
		eOmnCondVar,
		eOmnConnBuff,

		eOmnDbRecord,

		eOmnErrMsg,
		eOmnEventEntry,
		eOmnExcept,

		eOmnMsgComm,
		eOmnMutex,

		eOmnMemMnt,
		eOmnNetConn,
		eOmnNetEntity,
		eOmnNonExistObj,
		eOmnNotInDbObj,

		eOmnObjIds,

		eOmnPcObj,
		eOmnPcObjMgr,
		eOmnPresenceMgr,

		eOmnRealmImage,
		eOmnRepRealm,
		eOmnRouteEntry,

		eOmnScvsBranch,
		eOmnScvsLde,
		eOmnScvsPde,
		eOmnScvsPdeBase,
		eOmnScvsTag,
		eOmnScvsTagEntry,
		eOmnScvsView,
		eOmnScvsViewEntry,
		eOmnScvsVm,
		eOmnScvsVmLoc,
		eOmnSerialFrom,
		eOmnSerialTo,
		eOmnSoMemMtr,
		eOmnSoUniqueIds,
		eOmnSysObj,
		eOmnString,

		eOmnTestcase,
		eOmnTestMgr,
		eOmnTestRslt,
		eOmnTestSuite,
		eOmnThread,

		eOmnXmlItem,
		eOmnXmlParser,

		eAosUser,
        eAosRole,
        eAosUserBasedAC,
		eAosUserAuth,
        eAosUserBasedACL4,
        eAosUserBasedACVpn,

        eLastValidEntry
	};

private:
	static char	mClassIds[eLastValidEntry][eMaxClassIdLength];
	static bool mNamesInit;

public:
	static void initNames();

	static bool isValidMsg(const E e)
	{
		return isValid(e);
	}

	static bool isValid(E e)
	{
		return (e > eFirstValidEntry && e < eLastValidEntry);
	}

	static bool defineClassId(const E e, const char *name)
	{
		if (!mNamesInit)
		{
			initNames();
		}

		if (e < eFirstValidEntry || e >= eLastValidEntry)
		{
			return false;
		}

		if (mClassIds[e][0] == 0)
		{
			strncpy(mClassIds[e], name, eMaxClassIdLength-1);
		}
		return true;
	}

	static const char * const getClassId(const E e)
	{
		if (e < eFirstValidEntry || e >= eLastValidEntry)
		{
			return mClassIds[eOmnNonExistObj];
		}

		return mClassIds[e];
	}

};
#endif

