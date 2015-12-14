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
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataScanner_DirScanner_h
#define AOS_DataScanner_DirScanner_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/ReadFile.h"
#include "Rundata/Rundata.h"
#include "DataScanner/Ptrs.h"

class AosDirScanner : virtual public AosDataScanner
{
	OmnDefineRCObject;

	class AosReaderThrd : public OmnThreadedObj
	{
		OmnDefineRCObject;

		AosDataScannerObjPtr	mDirScanner;
		AosRundataPtr			mRundata;
		OmnThreadPtr			mThread;
	public:
		AosReaderThrd(
			const AosDataScannerObjPtr &dir_scanner,
			const AosRundataPtr &rdata)
		:
		mDirScanner(dir_scanner),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
			OmnThreadedObjPtr thisPtr(this, false);
			mThread = OmnNew OmnThread(thisPtr, "ReaderThrd", 0, true, true, __FILE__, __LINE__);
			mThread->start();
		}
		// ThreadedObj Interface
		virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
		virtual bool    signal(const int threadLogicId);
		bool    checkThread(OmnString &err, const int thrdLogicId) const;
	};

private:
	enum
	{
		eMaxBuffSize = 110000000,
		eMinBuffSize = 100000000
		//eMinBuffSize = 10000000
	};
	int64_t									mStart;
	int64_t									mReadLength;
	int										mPhysicalid;
	OmnString								mRowDelimiter;
	vector<OmnString>						mDirNames;
	vector<AosFileInfo>    				mFileList;
	u32										mCrtIdx;
	bool									mRecursion;
	OmnString								mCrtFileName;
	int64_t									mCrtFileLen;

	int										mLoopTmp;
	vector<AosDataScannerObjPtr>::iterator 	mCrtScannerItr;
	vector<AosDataScannerObjPtr> 			mScanners;
	OmnMutexPtr								mLock;
	OmnSemPtr								mReadToSem;
	OmnSemPtr								mReadFromSem;
	AosBuffDataPtr						mPrimaryBuff;
	bool									mNoMoreData;
	bool									mIsUnicomFile;
	bool									mIgnoreHead;
	OmnString								mCharacterType;
	OmnString								mRmDataDir;

public:
	AosDirScanner(const bool flag);
	AosDirScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDirScanner();

	virtual bool readBuffToQueue(OmnThrdStatus::E &state, const AosRundataPtr &rdata);
	virtual bool readBuffFromQueue(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual bool	isFinished(const AosRundataPtr &rdata);
	virtual bool	getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	// Chen Ding, 2013/11/25
	// virtual int64_t getTotalFileLength();
	virtual int64_t getTotalSize() const;
	virtual int64_t getTotalFileLengthByDir() 
	{ 
		int64_t len = 0;
		for (u32 i=0; i<mFileList.size(); i++)
		{
			len += mFileList[i].mFileLen;
		}
		return len;
	};
	virtual OmnString getCrtFileName(){ return mCrtFileName; }
	virtual AosJimoPtr cloneJimo()  const;
	virtual int getPhysicalId() const;

	bool initDirScanner(
			vector<OmnString> &dirnames,
			const int physical_id,
			const bool recursion,
			const bool ignore_head,
			const OmnString &character_type,
			const OmnString &row_delimiter,
			const AosRundataPtr &rdata);

	bool initDirScanner(
			vector<AosFileInfo> &fileinfos,
			const int physical_id,
			const bool ignore_head,
			const OmnString &character_type,
			const OmnString &row_delimiter,
			const AosRundataPtr &rdata);

	bool initUnicomDirScanner(
			vector<AosFileInfo> &fileinfos,
			const int physical_id,
			const OmnString &character_type,
			const OmnString &row_delimiter,
			const AosRundataPtr &rdata);

private:
	bool config(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);

	bool createDataScanner(
			vector<AosDataScannerObjPtr> &scanners,
			const vector<AosFileInfo> &filelist,
			const AosRundataPtr &rdata);

	char* strrstr(const char* s1, const int i, const char* s2);

	AosDataScannerObjPtr getScanner(const AosRundataPtr &rdata);

	bool readBuff(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	bool smartReadBuff(const AosRundataPtr &rdata, 
						AosBuffPtr &buff, 
						AosDiskStat &disk_stat);
	bool readWholeBuff(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	bool readUnicomFileBuff(AosBuffDataPtr &info, const AosRundataPtr &rdata);

	bool readXmlFindFile( 
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			AosBuffPtr &zip_buff,
			vector<AosBuffPtr> &new_buffs,
			vector<AosXmlTagPtr> &new_xmls);

	vector<AosXmlTagPtr> getDomainItem(
	        const AosXmlTagPtr &doc,
			int loopNum,
	        const AosRundataPtr &rdata);  

	OmnString unzip(OmnString &zipPath, const OmnString &fname);

	bool xmlParse(
			const AosRundataPtr &rdata,
			AosXmlTagPtr &xml);

};
#endif
#endif
