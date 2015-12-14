////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileObj.h
// Description:
//	A FileObj is either a file or a directory.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_FileObj_h
#define Snt_Util_FileObj_h

#include "Porting/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class OmnFileObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Type
	{
		eUnknown,
		eFile,
		eDirectory
	};

private:
	Type		mType;

public:
	OmnFileObj(const Type t)
		:
	mType(t)
	{
	}

	virtual ~OmnFileObj() {};
	
	Type		getType() const {return mType;}
	bool		isFile() const {return mType == eFile;}
	bool		isDirectory() const {return mType == eDirectory;}

};
#endif

