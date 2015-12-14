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
// 03/16/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ProgramAid_CreateClassFiles_h
#define Aos_ProgramAid_CreateClassFiles_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosCreateClassFiles : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
public:

private: 
	OmnString	mProjHomeDir;
	OmnString	mDirName;
	OmnString	mClassName;
	OmnString	mProjPrefix;
	bool		mSmartPtrSupport;
	bool		mForce;
	OmnString	mSrcFileExt;

public:
	AosCreateClassFiles(const OmnString &projHomeDir,
						const OmnString &dirname, 
						const OmnString &className, 
						const OmnString &projPrefix, 
						const bool withSmartPointerSupport, 
						const bool force,
						const OmnString &srcFileExt);
	virtual ~AosCreateClassFiles();

private:
	bool	createHeaderFile();
	bool	createSourceFile();
};
#endif

