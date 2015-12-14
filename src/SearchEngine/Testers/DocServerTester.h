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
// 2009/10/16	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_Testers_DocServerTester_h
#define Aos_SearchEngine_Testers_DocServerTester_h

#include "SearchEngine/DocServerCb.h"
#include "SEUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/Testers/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"


class AosXmlDoc;

class AosDocServerTester : public OmnTestPkg, public AosDocServerCb
{
private:
	enum
	{
		eMaxDocs = 3,
		eMaxWords = 10000,
		eMaxWordLen = 3000,
		eDepth = 5,
		eMaxFileSize = 0xefffffff,
		eNumVerifies = 5,
		eAInitSize = 5000,
		eAIncSize = 1000,
		eAMaxSize = 20000
	};

	OmnString			mDataDirname;
	AosTestXmlDocPtr	mDocs[eMaxDocs];
	u64					mDocIds[eMaxDocs];
	int					mNumDocs;
	int					mNumDocsSaved;
	u64					mWordIds[eMaxWords];
	OmnDynArray<OmnString, eAInitSize, eAIncSize, eAMaxSize>	mAllWords;
	OmnFilePtr			mCrtDocFile;
	int					mCrtFileIdx;
	u32					mCrtFilesize;
	OmnFilePtr			mCrtRawDocFile;
	int					mCrtRawFileIdx;
	u32					mCrtRawFilesize;
	u32					mWordIdHashTsize;
	u32					mTotalCreated;
	u32					mTotalProcessed;
	u64					mStartTime;
	u64					mDocid;

	OmnString 	mModifier;
	OmnString 	mCreator;
	OmnString 	mDocname;
	OmnString 	mThmnail;
	OmnString 	mDoctype;
	OmnString 	mSubtype;
	OmnString 	mAppname;

public:
	AosDocServerTester();
	~AosDocServerTester() {}

	virtual bool	start();
	virtual void	procFinished(const bool status, 
						const u64 &docId,
						const OmnString &errmsg, 
						const void *userdata, 
						const int numWords);	

private:
	bool	prepareEnv();
	bool	basicTest();
	bool 	torturer();
	bool 	verify();
	bool 	verifyOneDoc(const int);
	bool 	verifyAllWords();
	bool 	verifyDoc(const u64 &docId);
	bool	torturerXmlDocCreation();
	bool	saveDoc(const AosTestXmlDocPtr &, const u64 &);
	bool 	verifyRawDoc(const u64 docid);
	bool 	readXmlDocFromDb(
				const u64 &docid, 
				AosXmlDoc &header);
	bool 	readDocBodyFromFile(
				const AosXmlDoc &header,
				OmnString &body);
	bool 	saveHeader(const AosXmlDocPtr &header);
	bool 	addMetadata(const OmnConnBuffPtr &buff);
	bool 	verifyQuery();
};
#endif

