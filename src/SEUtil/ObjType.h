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
#ifndef AOS_SEUtil_ObjType_h
#define AOS_SEUtil_ObjType_h

#include "Util/String.h"

class AosObjType
{

public:
	enum E
	{
		eInvalid,

		eAccessRcd,
		eAlbum,
		eArticle,
		eBlog,
		eCloudCard,
		eComment,
		eContainer,
		eDefault,
		eDiary,
		eIdGenMgr,
		eImage,
		eLog,
		eLoginLog,
		ePublish,
		eSession,
		eSysBackupLog,
		eUserAccount,
		eUserGroup,
		eUserRole,
		eVersion,
		eVpd,

		eMax
	};

public:
	AosObjType();
	~AosObjType();

	static E convertToEnum(const OmnString &otype);
};
#endif

