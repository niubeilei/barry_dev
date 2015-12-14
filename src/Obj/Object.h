////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 03/26/2008: Copied from Util/Object.h by Chen Ding
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Obj_Object_h
#define Aos_Obj_Object_h

#include "alarm_c/alarm.h"
#include "util_c/types.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <string.h>

class AosObject : virtual public OmnRCObject
{
private:
	static u64 	mTotalActiveObjects;
	static u64	mTotalObjects;

//	AosDefineObjPreguard(__AosObject);
	std::string	mFilename;
	int			mLineno;
//	AosDefineObjPostguard(__AosObject);

public:
	AosObject();
	virtual ~AosObject();

	int setLocation(const char *file, const int line)
	{
		aos_assert_r(file, -1);
		aos_assert_r(line >= 0, -1);
		mFilename = file;
		mLineno = line;
		return 0;
	}

	int integrityCheck() const;

	std::string	getObjLocFileName() const {return mFilename;}
	int			getObjLocLineno() const {return mLineno;}
	
	static u64 	getTotalObjects() {return mTotalObjects;}	
	static u64 	getTotalActiveObjects() {return mTotalActiveObjects;}
};
#endif

