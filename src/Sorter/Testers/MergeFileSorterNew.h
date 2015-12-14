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
#ifndef Omn_TestUtil_MergeFileSorterNew_h
#define Omn_TestUtil_MergeFileSorterNew_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/CompareFun.h"
#include "Thread/ThrdShellProc.h"
#include "SEInterfaces/NetFileObj.h"
#include "Sorter/MergeFileSorter.h"
#include "Rundata/Rundata.h"


class AosMergeFileSorterNew : public OmnTestPkg
{
public:
	int 						mFileLen;
	OmnString   			 	mConf;
	AosRundataPtr				mRundata;
	AosCompareFunPtr		 	mCmpFun;
	vector<AosNetFileObjPtr> 	mVirFiles;
	AosMergeFileSorterPtr 		mSort;

private:
	int		mTries;

public:
	AosMergeFileSorterNew();
	~AosMergeFileSorterNew() {}

	virtual bool		start();

private:

	bool 		deleteFile();
	bool		testInsert();
	bool		createVirFiles(const AosRundataPtr &rdata);
	bool		createVirFiles();
	void 		createCmpFunc(const AosRundataPtr &rdata);
	bool 		sanitycheck(AosCompareFun * comp, char *crt, const int length);


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
				buff = mMergeSort->nextBuff();
			}
		}
		virtual bool procFinished()
		{
			return true;
		}
	};


};
#endif

