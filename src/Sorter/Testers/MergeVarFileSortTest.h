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
// 2013/02/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_MergeVarFileSortTest_h
#define Omn_TestUtil_MergeVarFileSortTest_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/CompareFun.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileObj.h"
#include "Sorter/MergeFileSorter.h"
#include "Thread/ThrdShellProc.h"


class AosMergeVarFileSortTest : public OmnTestPkg
{
public:
	int 						mFileLen;
	OmnString   			 	mConf;
	AosRundataPtr				mRundata;
	AosCompareFunPtr		 	mCmpFun;
	vector<AosNetFileObjPtr> 	mVirFiles;
	vector<AosNetFileObjPtr> 	mBigVirFiles;
	AosMergeFileSorterPtr 		mSort;
	AosMergeFileSorterPtr 		mMerge;

private:
	int							mTries;
	OmnString 					mSetValue;
	double						mDoubleValue;

public:
	AosMergeVarFileSortTest();
	~AosMergeVarFileSortTest() {}

	virtual bool		start();

	class AosGetOutputFile: public OmnThrdShellProc
	{
		OmnDefineRCObject;

	private:
		AosRundataPtr               mRundata;
		AosCompareFunPtr		 	mCmpFun;
		AosMergeFileSorterPtr 		mMergeSort;
		bool						mIsMerge;

	public:
		AosGetOutputFile(
				const AosMergeFileSorterPtr &merge_sort,
				const AosCompareFunPtr &cmo_fun,
				const bool is_merge,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("MutilFileSortThrd"),
		mMergeSort(merge_sort),
		mCmpFun(cmo_fun),
		mIsMerge(is_merge),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool run()
		{
			bool rslt = false;
			AosBuffPtr buff = mMergeSort->nextBuff();
			while (buff)
			{
				rslt = AosMergeFileSorter::sanitycheck_var(mCmpFun.getPtr(), buff->data(), buff->dataLen());
				aos_assert_r(rslt, false);
				if (mIsMerge)
				{
					rslt = AosMergeVarFileSortTest::sanitycheck(mCmpFun.getPtr(), buff->data(), buff->dataLen());
					aos_assert_r(rslt, false);
				}
				buff = mMergeSort->nextBuff();
			}
		}
		virtual bool procFinished()
		{
			return true;
		}
	};

private:

	bool		testSort();
	bool		createSortVirFiles(const AosRundataPtr &rdata);
	void		createSortCmpFunc(const AosRundataPtr &rdata);

	bool		testMerge();
	bool		createMergeVirFiles(const AosRundataPtr &rdata);
	void 		createMergeCmpFunc(const AosRundataPtr &rdata);
	static bool sanitycheck(AosCompareFun * comp, char *crt, const int length);
	bool 		deleteFile();
	bool 		deleteBigFile();

};
#endif

