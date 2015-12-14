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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerGen_TorturerGen_h
#define Omn_TorturerGen_TorturerGen_h

#include "TorturerGen/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"


class AosTorturerGen : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mFilename;
	OmnString		mSrcDirName;
	OmnString		mTestDirName;
	OmnString		mTorturerName;
	OmnString		mTorturerDesc;
	bool			mWithCliSupport;
	OmnDynArray<AosTesterGenPtr>	mTesters;

public:
	AosTorturerGen(
		const OmnString &sirDirName, 
		const OmnString &testerDirName,
		const OmnString &torturerName, 
		const OmnString &filename, 
		const bool withCliSupport, 
		const bool createBasicTester);
	AosTorturerGen(const OmnString &filename);
	virtual ~AosTorturerGen();

	bool		readConfig();
	bool		genCode(const bool force);
	OmnString	getDirectoryName() const;
	bool		addTester(const AosTesterGenPtr &tester);


private:
	bool	createTesters(const bool force);
};

#endif

