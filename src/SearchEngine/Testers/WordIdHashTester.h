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
// 2009/10/09	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_Testers_WordIdHashTester_h
#define Aos_SearchEngine_Testers_WordIdHashTester_h

#include "Tester/TestPkg.h"
#include "Util/HashStrVal.h"


class AosWordIdHashTester : public OmnTestPkg
{
	enum
	{
		eWordSize = 100
	};

private:
	OmnString	mDirname;
	OmnString	mHashFname;
	u32			mWordIdHashTsize;
	int			mNumQueries;
	OmnString	mWords[eWordSize];
	OmnHashStrVal<OmnString, u64, 100000>	mWordIds;
	OmnHashStrVal<OmnString, u64, 100000>	mPtrs;
	int			mWordsRetrieved;

public:
	AosWordIdHashTester() {mName = "SearchEngineTester";}
	~AosWordIdHashTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool 	torture();
	bool 	verifyDb();
	bool 	queryTable(
				char *all_words,
				const int numWords);
	OmnString	generateNewWord();
	bool 	prepareEnv();
	bool 	wordExist(
				const OmnString &word, 
				u64 &wordId);
	bool 	getWordsFromDb(
				const int numWords, 
				OmnString words[eWordSize],
				u64 wordIds[eWordSize],
				u64 ptrs[eWordSize],
				int &numWordsRetrieved);
	bool 	queryTable(const int numWords);
};


#endif

