////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SingletonObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SingletonClass_SingletonObj_h
#define Omn_SingletonClass_SingletonObj_h

#include "SingletonClass/SingletonObjId.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "XmlUtil/Ptrs.h"

class OmnSingletonObj
{
public:
	enum ObjId
	{
		eTimer,
		eTagMgr
	};

private:
	OmnSingletonObjId::E	mId;

public:
	OmnSingletonObj(const OmnSingletonObjId::E id)
	:
	mId(id)
	{
	}
	virtual ~OmnSingletonObj() {}	

	virtual bool					createSingleton() = 0;
	virtual bool					stopSingleton() = 0;
	virtual bool					config(const AosXmlTagPtr &def) = 0;
	virtual OmnString   getSysObjName() const = 0;

	OmnSingletonObjId::E	getSysObjId() const {return mId;}
};

#endif
