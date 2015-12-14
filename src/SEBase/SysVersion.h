////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEBase_SysVersion_h
#define Omn_SEBase_SysVersion_h

class AosSysVersion
{
public:
	enum
	{
		eSysVersion_1 = 1,

		eVfsVersion_1 = 1,
		eVfsVersion_2 = 2
	};

private:
	static int		mCrtSysVersion;
	static int		mCrtVfsVersion;

public:
	static void setCrtSysVersion(const int v) {mCrtSysVersion = v;}
	static void setCrtVfsVersion(const int v) {mCrtVfsVersion = v;}

	static int  getCrtSysVersion() {return mCrtSysVersion;}
	static int  getCrtVfsVersion() {return mCrtVfsVersion;}
};
#endif
