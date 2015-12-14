////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/13/2015 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataAssembler_DataAssemblerSort_h
#define AOS_DataAssembler_DataAssemblerSort_h

#include "DataAssembler/DataAssembler.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Sorter/MergeFileSorter.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/BuffArray.h"
#include "Util/CompareFun.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/ThrdShellProc.h"
#include "DataSort/DataSort.h"

#include <vector>


class AosDataAssemblerSort : public AosDataAssembler
{
	enum
	{
		eNumFilesToMerge = 100
	};
private:
	class AosSaveFileThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		private: 
		AosDataAssemblerObjPtr 		mAssembler;
		OmnString					mFileName;
		AosBuffPtr					mBuff;
		AosRundataPtr 				mRundata;
		public:
		AosSaveFileThrd(
				const AosDataAssemblerObjPtr &asembler,
				const OmnString &fname,
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("SaveFileThrd"),
		mAssembler(asembler),
		mFileName(fname),
		mBuff(buff),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		virtual bool run();
		virtual bool procFinished();
	};

	class AosMultiFileSortThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		private:
		AosDataAssemblerObjPtr 		mAssembler;
		AosCompareFun*	        	mCompRaw;
		OmnLocalFilePtr				mOutPutFile;
		vector<AosNetFileObjPtr>	mNetFiles;
		AosRundataPtr 				mRundata;
		public:
		AosMultiFileSortThrd(
				const AosDataAssemblerObjPtr &assembler,
				AosCompareFun* comp_raw,
				const OmnLocalFilePtr &output_file,
				const vector<AosNetFileObjPtr> &net_files,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("MutilFileSortThrd"),
		mAssembler(assembler),
		mCompRaw(comp_raw),
		mOutPutFile(output_file),
		mNetFiles(net_files),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		virtual bool run();
		virtual bool procFinished();
	};

	class AosGenerateOutPutThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		private:
		AosDataAssemblerObjPtr 		mAssembler;
		AosMergeFileSorterPtr		mSorter;
		vector<AosNetFileObjPtr>	mNetFiles;
		OmnLocalFilePtr				mOutPutFile;
		AosRundataPtr 				mRundata;
		public:
		AosGenerateOutPutThrd(
				const AosDataAssemblerObjPtr &assembler,
				const AosMergeFileSorterPtr &sorter,
				const vector<AosNetFileObjPtr> &net_files,
				const OmnLocalFilePtr &output_file,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("GenerateOutputThrd"),
		mAssembler(assembler),
		mSorter(sorter),
		mNetFiles(net_files),
		mOutPutFile(output_file),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		virtual bool run();
		virtual bool procFinished();
	};


private:
	i64						mNumEntry;
	i64						mBuffMaxSizeToSend;

	AosCompareFunPtr		mComp;
	AosCompareFun *			mCompRaw;

	AosBuffArrayPtr			mArray;
	AosBuffArray *			mArrayRaw;

	AosBuffArrayVarPtr		mArrayVar;
	AosBuffArrayVar*		mArrayVarRaw;
	u64						mTaskDocid;
	AosDataRecordType::E	mRecordType;

	u64						mJobDocid;
	OmnString				mTaskId;
	vector<u64>				mFiles;
	u64						mTmpFileSeqno;
	u64						mOutPutSeqno;
	u32						mTotalFiles;
	OmnSemPtr       		mTmpFileSem;
	OmnSemPtr       		mOutPutSem;
	OmnSemPtr				mStreamSem;
	OmnSemPtr				mMergeFileSem;

public:
	AosDataAssemblerSort(
			const OmnString &asm_key,
			const u64 task_docid);
	~AosDataAssemblerSort();

	virtual bool sendStart(AosRundata *rdata);

	virtual bool sendFinish(AosRundata *rdata);
	virtual bool waitFinish(AosRundata *rdata);

	virtual bool appendEntry(
					AosRundata *rdata,
					AosDataRecordObj *record);

	virtual bool appendSortedFile(
					const u64 &file_id, 
					const AosRundataPtr &rdata);
	virtual void tmpFilePost();
	virtual void outPutPost();
	virtual void streamPost();
	virtual void mergeFilePost();


	bool config(
					const AosXmlTagPtr &def,
					AosRundata *rdata);
	static bool checkConfigStatic(
			const AosXmlTagPtr &def,
			AosRundata* rdata);

	static AosDataAssemblerObjPtr createAssemblerSort(
					const OmnString &asm_key, 
					const u64 task_docid, 
					const AosXmlTagPtr &def, 
					AosRundata* rdata);
private:
	bool sortAndFlush(const AosBuffArrayPtr &array, const AosRundataPtr &rdata);
	bool sortAndFlush(const AosBuffArrayVarPtr &arrayVar, const AosRundataPtr &rdata);
	bool flush(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool mergeFiles(const vector<u64> &files, const AosRundataPtr &rdata);
	OmnString createTmpFileName()
	{
		int seqno;
		mLockRaw->lock();
		seqno = mTmpFileSeqno++;
		mLockRaw->unlock();
		OmnString fname = "tmp_";
		fname << mJobDocid << "_" << mTaskId << "_" << mAsmKey << "_" << seqno;
		return fname;
	}

	OmnString createFileName()
	{
		int seqno;
		mLockRaw->lock();
		seqno = mOutPutSeqno++;
		mLockRaw->unlock();
		OmnString fname = "mg_";
		fname << mJobDocid << "_" << mTaskId << "_" << mAsmKey << "_" << seqno;
		return fname;
	}
	bool sanitycheck(char* begin, int length);

	void showDataAssemblerInfo(
			const char *file,
			const int line,
			const u32 total_outputs);
};
#endif

