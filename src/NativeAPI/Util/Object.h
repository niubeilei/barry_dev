////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Object.h
// Description:
//	This is the super class for all objects, including programming 
//  objects, data centric objects, system objects, etc.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Object_h
#define Omn_Util_Object_h

#include "Obj/ClassId.h"
#include "Porting/LongTypes.h"
#include "Util/RCObject.h"
#include <string.h>

class OmnObject : virtual public OmnRCObject
{
private:
	enum
	{
		eMaxFileLengthDefinedInObj = 100, 

		eObjectSecretFlag1 = 0xa1b2c3d4,
		eObjectSecretFlag2 = 0x9a8b7c6d
	};

	static OmnInt64	mTotalActiveObjectCount;
	static OmnInt64	mTotalObjectCount;
	static OmnInt64	mClassActiveObjCount[OmnClassId::eLastValidEntry];
	static OmnInt64	mClassTotalObjCount[OmnClassId::eLastValidEntry];
	static bool		mCountObjectsFlag;
	static bool		mObjectCountersInit;

	unsigned int	mObjectSecretFlag1;

	OmnClassId::E	mClassId;
	char			mObj_File_Name[eMaxFileLengthDefinedInObj+1];
	int				mObj_File_Line;
	
	unsigned int	mObjectSecretFlag2;

public:
	OmnObject(const OmnClassId::E classId = OmnClassId::eOmnUnknown);
	virtual ~OmnObject();

	// 
	// OmnObj interface
	//
	virtual int				objSize() const = 0;

	OmnClassId::E	getClassId() const {return mClassId;}
	OmnObject & setLocation(const char *file, const int line)
	{
		strncpy(mObj_File_Name, file, eMaxFileLengthDefinedInObj);
		mObj_File_Name[eMaxFileLengthDefinedInObj] = 0;
		mObj_File_Line = line;
		return *this;
	}

	bool isObjGood() const 
	{
		return mObjectSecretFlag1 == eObjectSecretFlag1 &&
			   mObjectSecretFlag2 == eObjectSecretFlag2;
	}

	const char *	getObjLocationFileName() const {return mObj_File_Name;}
	int				getObjLocationLine() const {return mObj_File_Line;}
	
	static OmnInt64	getTotalObjectCount() {return mTotalObjectCount;}	
	static OmnInt64	getTotalActiveObjectCount() {return mTotalActiveObjectCount;}
	static void		initObjectCounters();
	static OmnInt64	getClassActiveObjCount(const OmnClassId::E classId);
	static OmnInt64	getClassTotalObjCount(const OmnClassId::E classId);
	static void		startCount() {mCountObjectsFlag = true;}
	static void		stopCount() {mCountObjectsFlag = false;}
	
};
#endif
