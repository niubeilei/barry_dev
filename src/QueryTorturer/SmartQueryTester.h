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
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_SmartQueryTester_h
#define Aos_QueryTorturer_SmartQueryTester_h

#include "QueryTorturer/Ptrs.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
#include <list>
using namespace std;

class AosSmartQueryTester : public OmnTestPkg, public OmnThreadedObj
{
	typedef hash_map<const u64, AosQueryGroupPtr, u64_hash, u64_cmp> gmap_t;
	typedef hash_map<const u64, AosQueryGroupPtr, u64_hash, u64_cmp>::iterator gmapitr_t;

	enum
	{
		eDftNumStrFields = 5,
		eDftNumU64Fields = 0,
		eNumStrings = 1000,
		eDftTries = 1000000,
		eBuffLength = 1000000,
		eDftBatchSize = 100000,
		eDftRecordLen = 30,
		eDftMinBlockSize = 5000000,
		eDftMaxBlockSize = 20000000,
		eDftMaxGroups = 100,
		eDftMaxSeedSize = 100
	};

private:
	AosRundataPtr			mRundata;
	int						mRecordLen;
	int						mTries;
	int						mNumStrings;
	int						mPagesize;
	i64						mNumDocs;
	int						mNumStrFields;
	int						mNumU64Fields;
	int						mMinBlockSize;
	int						mMaxBlockSize;
	int						mMaxGroups;
	u64						mStartDocid;
	u32						mCrtGroupId;
	bool					mTestFinished;
	int						mNumGroupsToTry;
	int						mNumGroupsTried;
	int						mFieldValueMethod;
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	u64						mMinGroupId;
	int						mMaxActiveGroups;
	int						mMaxTimeField;
	int						mMinTimeField;
	int						mMaxBuffSize;
	OmnString				mTableName;
	OmnString				mDatabase;
	OmnString				mTestType;
	int						mNumThreadsPerField;
	gmap_t					mGroups;
	vector<AosQueryFieldPtr>	mFields;
	OmnThreadPtr			mGroupCreationThread;

public:
	AosSmartQueryTester();
	~AosSmartQueryTester();

	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

	int getMaxTimeField() const {return mMaxTimeField;}
	int getMinTimeField() const {return mMinTimeField;}
	void fieldDataCreationFinished();
	int getNumFinishedCreators();
	int getTotalCreators();
	int getMinBlockSize() const {return mMinBlockSize;}
	int getMaxBlockSize() const {return mMaxBlockSize;}
	int getRecordLen() const {return mRecordLen;}
	u64 getStartDocid() const {return mStartDocid;}
	int getFieldValueMethod() const {return mFieldValueMethod;}
	int getMaxBuffSize() const {return mMaxBuffSize;}
	int getPagesize() const {return mPagesize;}
	u64 getMaxDocid() const;
	u64 getMinDocid() const{return mStartDocid;}
	int64_t getNumDocs() const {return mNumDocs;}
	bool groupFinished(const u64 group_id);
	bool groupFailed(const u64 group_id);
	u32 getNumFields();
	vector<AosQueryFieldPtr> getFields();
	bool runQuery();
	bool grandQueryTorturer();
	bool setFields();

	bool start();
	static OmnString getResult();

	virtual bool queryFinished(
	            const AosRundataPtr &rdata,
	            const AosQueryRsltObjPtr &results,
	            const AosBitmapObjPtr &bitmap,
	            const AosQueryProcObjPtr &proc) {return false;}

private:
	bool	init();
	bool	config();
};

#endif

#if 0
	enum
	{
	};

	struct MapEntry
	{
		u32 mPoison;
		vector<AosBitmapExecutor::InsertBitsEntry> entries;

		MapEntry()
		:
		mPoison(1234567)
		{
		}

		~MapEntry()
		{
			mPoison = 0;
		}

		void push(const AosBitmapExecutor::InsertBitsEntry &entry)
		{
			entries.push_back(entry);
		}

		bool check(const u64 bitmap_id)
		{
			for (u32 i=0; i<entries.size(); i++)
			{
				if (entries[i].bitmap_id == bitmap_id)
				{
					entries.erase(entries.begin() + i);
					return true;
				}
			}

			return false;
		}

		bool isEmpty() const
		{
			return entries.size() == 0;
		}
		
		int getIILLevel() const
		{
			aos_assert_r(entries.size() > 0, 0);
			return entries[0].iil_level;
		}

		int getNodeLevel() const
		{
			aos_assert_r(entries.size() > 0, 0);
			return entries[0].node_level;
		}

		u64 getNodeId() const
		{
			aos_assert_r(entries.size() > 0, 0);
			return entries[0].node_id;
		}
		
		u64 getBitmapId() const
		{
			aos_assert_r(entries.size() > 0, 0);
			return entries[0].bitmap_id;
		}
	};

public:
	typedef hash_map<const u64, u64, u64_hash, u64_cmp> imap_t;
	typedef hash_map<const u64, u64, u64_hash, u64_cmp>::iterator imapitr_t;
	typedef hash_map<const u64, MapEntry, u64_hash, u64_cmp> mmap_t;
	typedef hash_map<const u64, MapEntry, u64_hash, u64_cmp>::iterator mmapitr_t;

private:
	AosRandomBuffArrayPtr	mBuffArray;
	OmnString				mIILName;
	AosCompareFunPtr		mCompareFunc;
	int						mBatchSize;
	int						mKeyLen;
	AosBitmapTreePtr		mTree;
	AosBitmapObjPtr			mLastBitmap;
	AosActionObjPtr			mAction;
	vector<u64>				mFields1;
	vector<u64>				mFields2;
	vector<u64>				mFields3;
	vector<u64>				mFields4;
	vector<u64>				mFields5;
	vector<u64>				mFields6;
	vector<u64>				mFields7;
	vector<u64>				mFields8;
	vector<u64>				mFields9;
	vector<u64>				mFields10;
	OmnString				mCrtKey;
	u64						mCrtValue;
	u64						mTotalEntries;
	u64						mNumQueried;
	AosQueryReqObjPtr		mQueryReq;
	AosXmlTagPtr			mQueryDef;
	int64_t					mCrtQueryPos;
	int						mMinSeedSize;
	int						mMaxSeedSize;
	bool					mGroupTorturing;
	int						mNumActiveGroups;
	AosQueryGroupPtr		mQueryGroup;
	vector<u64>				mLeafIds;
	vector<u64>				mLevelOneIds;
	int						mNumFinishedCreators;
	int						mTotalCreators;

public:
	AosSmartQueryTester();
	~AosSmartQueryTester();

	virtual bool		start();


	int getNumStrFields() const {return mNumStrFields;}
	int getNumU64Fields() const {return mNumU64Fields;}
	int getMinSeedSize() const {return mMinSeedSize;}
	int getMaxSeedSize() const {return mMaxSeedSize;}
	OmnString getStrIILName(const int field_idx);
	OmnString getU64IILName(const int field_idx);
	u64 getStrValue(const int field_idx, const u64 docid);
	u64 getU64Value(const int field_idx, const u64 docid);
	u64 createFile( const AosBuffPtr &buff, const AosRundataPtr &rdata);
	AosQueryFieldPtr getField(const int idx);

private:
	bool	basicTest();
	bool	rebuildBitmap();
	bool 	addContents();
	bool 	checkResults();
	bool	verifyLeaves();
	bool	verifyLevelOneNodes();
	bool	verifyInsertBits();
	bool	verifyTempBits();
	bool	verifyRebuildNodes();
	bool 	verifyContents(const int start,const int end);
	bool 	verifyLeavesAndLevelOneNodes();
	bool 	retrieveLevelOneNodes();
	bool 	verifyIILs();
	bool 	verifyOneIIL(const AosIILObjPtr &iil);
	OmnString getKey(const char *seed, const u64 &docid);
	bool 	verifyTreeStructure();
	bool 	verifyLevelOneNodes2();
	int 	getLeafIdx(const u64 &leaf_id);
	bool 	getLevelOneNodes(vector<u64> &level_one_nodes);
	bool 	verifyInsertBitsPath();
	bool 	verifyCreateBitmaps();
	bool 	checkAddBits(const u64 &parent_id, const u64 &bitmap_id);
	AosBitmapObjPtr retrieveLeaf(
					const u64 &parent_id, 
					const u64 &leaf_id);
	
	u32 	getVirtualId();
	AosBitmapExecutorPtr getExecutor() const;
	bool getInsertBits(const AosRundataPtr &rdata, vector<AosBitmapObjPtr> &bitmaps);
	// bool createDocs();
	// bool createStrFields();
	bool createU64Fields();
	bool createOneStrField(const int field_idx);
	bool createOneU64Field(const int field_idx);
	bool createGroups();
	bool createOneGroup(const u64 group_id);
	OmnString getGroupIdIILName(const int group_id);
	bool queryGroup();
	bool saveQueryStat();
	bool groupQueryFinished();
	bool runTestCase();
	bool testFinished() const
	{
		return mNumGroupsTried >= mNumGroupsToTry;
	}
	bool readFields();
	bool saveFields();
	bool checkBitmapCreation();
	bool verifyFinalResults();
	bool verifyRemoteNodes();
	bool verifyOneRemoteNode(const u64 node_id, const vector<u64> child_list, const int level);
	AosBitmapObjPtr retrieveBitmap(
			const u64 node_id, 
			const int level, 
			const vector<u32> &sections);
	AosBitmapObjPtr retrieveRemoteBitmap(
			const u64 node_id, 
			const int level, 
			const vector<u64> &sections);
	bool verifyParentBits(
			const u64 iil_id, 
			const u64 *docids, 
			const int num_docs, 
			const vector<u32> &sections, 
			OmnString &log);
	bool getParentBitmaps(
			const u64 iilid,
			const u64 section_id,
			vector<AosBitmapObjPtr> &parent_bitmaps);
	bool verifyOneDocid(const u64 docid, OmnString &log);
	bool verifyOneDocid(const AosBitmapObjPtr &bitmap, const u64 docid);
	bool verifyTreeQueryOnce();
	bool verifyTreeQueryNode();
	// bool createOneField(const int fidx);

	// Chen Ding, 2013/10/26
	bool createContentsOnly();
	bool createOneFieldDataOnlyRunner(
			vector<OmnThrdShellProcPtr> &runners, 
			const AosQueryFieldPtr &field);
	bool createOneFieldFromData(const int field_id);
	void checkMemory();
	bool mergeFiles(const int field_idx);
	bool createIndexEntries();

public:
	void logIILCreation(const OmnString &stat);

};
#endif

