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
//
// Modification History:
// 03/01/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerGen_SimpleTesterGen_h
#define Omn_TorturerGen_SimpleTesterGen_h

#include "TorturerGen/Ptrs.h"
#include "TorturerGen/TesterGen.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"

class OmnFile;

class AosSimpleTesterGen : public AosTesterGen
{
	OmnDefineRCObject;

private:
	int			mTries;
	OmnString	mMemberData;
	OmnString	mHeaderIncludes;
	OmnString	mSourceIncludes;

public:
	AosSimpleTesterGen(const OmnString &srcDirName, 
					   const OmnString &testDirName, 
					   const OmnString &filename,
					   const int tries = 10000);
	virtual ~AosSimpleTesterGen();

	virtual bool	generateCode(const bool force);

	bool		config(const OmnXmlItemPtr &def);

private:
	bool	createTests(const OmnXmlItemPtr &def);
	bool 	addMemberData(OmnFile &file);
	bool 	addMemberFunctionToHeaderFile(OmnFile &file);
	bool	createHeaderFile(const bool force);
	bool	createSourceFile(const bool force);
};

#endif

