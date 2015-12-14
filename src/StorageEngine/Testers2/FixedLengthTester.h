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
// 2013/03/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_Testers2_FixedLengthTester_h
#define Aos_StorageEngine_Testers2_FixedLengthTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosFixedLengthTester : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	enum
	{
		//eDftNumRanges = 10000,
		eDftNumRanges = 1,
		eDftDocidRangeSize = 10 * 1000 * 1000,		// 10M
		eDftMaxDocidsPerTry = 50000,
		eDftNumChecksPerTry = 30000,
		eDftRecordLength = 81,

		eDftWeightAddContents = 100,
		//eDftWeightCheckContents = 100,
		eDftWeightCheckContents = 5,
		eDftBuffLen = 10000000,// 10M
		eDftTries = 100000,
		eStartSizeid = 10,
		eDftServerId = 0
	};

	struct AosDocids
	{
		u64 	start_docid;
		int		num_docids;
		u32		crt_sizeid;
		int		crt_index;

		vector<u64> 	save_sdocid;
		vector<int> 	save_numid;
		AosDocids(const u64 &id, const int &nn, const u32 &sizeid)
		:
		start_docid(id),
		num_docids(nn),
		crt_sizeid(sizeid),
		crt_index(0)
		{
			setSaveDocids(start_docid);
		}

		void setSaveDocids(const u64 &start_docid)
		{
			save_sdocid.push_back(start_docid);
			save_numid.push_back(0);
			crt_index = save_sdocid.size()- 1;
		}

		void updateSaveNumId(const int &num_docids)
		{
			aos_assert(crt_index < save_numid.size());
			save_numid[crt_index] += num_docids;
		}

		u64 getSaveDocids(const int idx) 
		{
			aos_assert_r(idx <= crt_index, 0);
			return save_sdocid[idx];
		}

		u64 getSaveNumid(const int idx)
		{
			aos_assert_r(idx <= crt_index, 0);
			return save_numid[idx];
		}
	};

	int 				mTries;
	OmnString			mSep;
	OmnString			mField1;
	OmnString			mField2;
	int					mDocidRangeSize;
	int					mMaxDocidsPerTry;
	map<u32, AosDocids *>	mDocids;	
	int					mNumChecksPerTry;
	int					mRecordLength;

	int					mWeightAddContents;
	int					mWeightCheckContents;

	AosXmlTagPtr 		mRecordDoc;
	vector<u64> 		mSizeid;

	AosDataAssemblerObjPtr		mDocAssembler;

	OmnString			mTaskDocStr[100];
	bool				mSaveDocFileMgrFinished;
	bool				mCreateFileFinished;
	int					mNumObjid;

public:
	AosFixedLengthTester(
			const AosXmlTagPtr &record_doc,
			const int &recordlength,
			const AosRundataPtr &rdata);

	~AosFixedLengthTester();

	bool	basicTest(const AosRundataPtr &rdata);

private:

	bool	addContents(const AosRundataPtr &rdata);

	bool	checkResults(const AosRundataPtr &rdata);

	bool 	checkDoc(const u64 &docid, const AosRundataPtr &rdata);

	bool 	saveDocids(const AosRundataPtr &rdata);

	int 	pickOperation(const vector<int> &weights);

	bool 	saveDoc(const AosRundataPtr &rdata);

	void 	saveToFile(const AosRundataPtr &rdata);

	void 	clear(const AosRundataPtr &rdata);

	bool 	saveAndCheck(const AosRundataPtr &rdata);

public:
	void 	updateOutPut(
				const int serverid,
				const OmnString &key,
				const u32 sizeid,
				const int docsize,
				const u64 &file_id);

	void 	setCreateFileFinished()
	{
		mCreateFileFinished = true;
	}

	void 	setSaveDocFileMgrFinished()
	{
		mSaveDocFileMgrFinished = true;
	}
};
#endif
