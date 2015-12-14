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
// 07/26/2010	Created by James Kong
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_Testers_DiskBlockTester_h
#define Aos_SEUtil_Testers_DiskBlockTester_h

#include "SearchEngine/Ptrs.h"
#include "SEClient/Testers/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"


class AosXmlDoc;

class AosDiskBlockTester : public OmnTestPkg
{
private:
	enum
	{
		eCreateWeight = 20,
		eDeleteWeight = 25
	};

	int		mRepeatLevel1;
	int		mRepeatLevel2;
	int		mRepeatLevel3;
	int		mRepeatLevel4;
	int		mRepeatLevel5;

public:
	AosDiskBlockTester();
	~AosDiskBlockTester() {}

	virtual bool	start();

private:
	bool	prepareEnv();
	bool	normalTest();
	bool 	createDoc();
	bool 	deleteDoc();
	bool 	modifyDoc();
	bool 	readDoc();
	int		pickRepeat();
	u64     allDoc();
	bool 
	setContents(
		char *data, 
		const int repeat, 
		const int pattern_len,
		char *pattern);
};
#endif

