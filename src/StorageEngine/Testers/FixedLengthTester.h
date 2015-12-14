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
#ifndef Aos_StorageEngine_Testers_FixedLengthTester_h
#define Aos_StorageEngine_Testers_FixedLengthTester_h

//#include "StorageEngine/Ptrs.h"
#include "StorageEngine/Testers/Ptrs.h"
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
		eDftWeightCheckContents = 30,
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

	int 		mTries;
	OmnString	mSep;
	OmnString	mField1;
	OmnString	mField2;
	int			mDocidRangeSize;
	int			mMaxDocidsPerTry;
	vector<AosDocids *>	mDocids;	
	int			mNumChecksPerTry;
	int			mRecordLength;

	int			mWeightAddContents;
	int			mWeightCheckContents;

	AosXmlTagPtr mRecordDoc;
	u64			mRecordDocid;
	u64 		mTotalNumDocs;

	u64 		mCrtJobId;

	bool		mSendStart;
	bool		mSendFinish;
	bool		mSendCleanData;
	bool		mStartCheck;
	vector<AosDocInfo3Ptr>	mDocInfos;

	u32			mCrtSizeid;
	u64			mAllTotalNumDocs;
	vector<u64> mSizeid;
	vector<int> mServer;

public:
	AosFixedLengthTester(
			const AosXmlTagPtr &record_doc,
			const int &recordlength,
			const u64 &start_jobid,
			const AosRundataPtr &rdata);

	~AosFixedLengthTester();

	bool	basicTest(const AosRundataPtr &rdata);

	void 	finish(const AosRundataPtr &rdata);

	void 	docFinished();

	void  	setNumChecksPerTry(const u64 &n){mNumChecksPerTry = n;}

	void  	setWeightCheckContents(const u64 &n) {mWeightCheckContents = n;}

	void	updateServer(const u32 serverid);

	bool 	checkServerFinished();
private:

	void	clear();
	bool	config();

	bool	addContents(const AosRundataPtr &rdata);

	bool 	addContents(
			const u64 &start_docid, 
			const u32 num_docids,
			const AosRundataPtr &rdata);

	bool	checkResults(const AosRundataPtr &rdata);

	bool 	checkDoc(const u64 &docid, const AosRundataPtr &rdata);

	bool 	sendContents();

	bool 	getNextBlockDocid(const AosRundataPtr &rdata);

	bool 	getMoreDocids(u64 &docid, int &num_docids, u32 &sizeid, bool &overfl, const AosRundataPtr &rdata);

	bool	sendContents(
				const AosBuffPtr &contentBuff,
				const AosBuffPtr &docidBuff,
				const u64 &req_num_docids,
				const u32 &sizeid,
				const AosRundataPtr &rdata);

	bool startData(const AosRundataPtr &rdata);
	bool finishData(const AosRundataPtr &rdata);
	bool cleanData(const AosRundataPtr &rdata);
	bool saveDocids(const AosRundataPtr &rdata);

	int pickOperation(const vector<int> &weights);

	bool saveDoc(const AosRundataPtr &rdata);
};
#endif
