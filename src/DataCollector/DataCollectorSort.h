////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataCollector_DataCollectorSort_h
#define AOS_DataCollector_DataCollectorSort_h

#include "API/AosApi.h"
#include "DataCollector/Ptrs.h"
#include "DataCollector/DataCollector.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"

#include <math.h>
#include <vector>
#include <set>

using namespace std;

class AosDataCollectorSort : virtual public AosDataCollector 
{

private:
	class AosOutFileMgr
	{
	public:
		vector<int> 	mFilesNum;
	public:
		AosOutFileMgr()
		{
			int phy_num = AosGetNumPhysicals();
			mFilesNum.resize(phy_num);
		}
		
		void clear() {mFilesNum.clear();}
		void add(int phy_id)
		{
			mFilesNum[phy_id] ++;
		}

		int getPhyId()
		{
			int phy_id = 0;
			for (u32 i = 0; i< mFilesNum.size(); i++)
			{
				if (mFilesNum[phy_id] < mFilesNum[i])
					phy_id = i;
			}
			return phy_id;
		}
	};

private:
	bool					mIsNeedFinish;
	bool					mIsMergeAll;
	vector<AosXmlTagPtr> 	mLastMergeFiles;
	vector<set<u64> >		mActiveTaskDocids;
	set<u64>				mCallBackTaskDocids; // for debug

public:
	AosDataCollectorSort(const u64 &job_id);
	~AosDataCollectorSort();
	
	virtual bool addOutput(
				const AosXmlTagPtr &output, 
				const AosRundataPtr &rdata);

	virtual bool config(const AosJobObjPtr &job, const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	virtual bool callBack(const AosXmlTagPtr &output, const AosRundataPtr &rdata);
	virtual bool finishDataCollector(const AosRundataPtr &rdata);
	virtual bool createMergeFileTask(
				const int level,
				const AosXmlTagPtr &conf,
				list<AosXmlTagPtr>& outfiles,
				u64 &task_id,
				const AosRundataPtr &rdata);
	//Jozhi 21015-03-09
	virtual bool insertActiveTask(const AosXmlTagPtr &task_doc);
	virtual bool reStartLogicTask(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata);
	virtual bool reStartDataCol(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata);
	virtual bool 	cleanDataCol(
						const u64 &task_docid,
						const AosRundataPtr &rdata);
	virtual bool 	eraseActiveTask(const AosXmlTagPtr &task_doc);

private:
	bool dataCollectorFinished(const AosRundataPtr &rdata);
	bool addOutputPriv(const AosRundataPtr &rdata, const AosXmlTagPtr &output, const int &level);
	bool mergeFiles(const int &level, const AosRundataPtr &rdata);

	bool checkLastMerge(const int &level);
	bool checkActiveTaskIsEmpty();
	bool checkActiveTask(const int &level);
	int getNextActiveTaskLevel(const int &level);
	int getNextNotEmptyLevel(const int &level);
	bool createTaskDocPriv(
				u64 &task_docid,
				list<AosXmlTagPtr> &info_list,
				const int level,
				const AosJobObjPtr &job,
				const AosXmlTagPtr &datacol,
				const AosRundataPtr &rdata,
				const int block_idx = 0,
				const bool is_last_merge = false);
	void showDataCollectorFinish();
	void showDataCollectorTotalFinish();
};
#endif
