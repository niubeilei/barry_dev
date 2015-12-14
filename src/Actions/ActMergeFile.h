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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActMergeFile_h
#define Aos_SdocAction_ActMergeFile_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "API/AosApi.h"
#include "Rundata/Ptrs.h"
#include "Sorter/MultiFileSorter.h"
#include "Sorter/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Sorter/MergeFileSorter.h"


class AosActMergeFile : virtual public AosSdocAction
{
private:
	class AosGenerateOutPutThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosActionObjPtr				mCaller;
		AosMergeFileSorterPtr		mSorter;
		OmnLocalFilePtr 			mOutPutFile;
		AosRundataPtr 				mRundata;
		AosBuffPtr				mTmpBuff; // for sanity check

	public:
		AosGenerateOutPutThrd(
				const AosActionObjPtr &caller,
				const AosMergeFileSorterPtr &sorter,
				const OmnLocalFilePtr &output_file,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("GenerateOutPutThrd"),
		mCaller(caller),
		mSorter(sorter),
		mOutPutFile(output_file),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};
	AosDataRecordType::E			mRecordType;
	OmnSemPtr       				mOutPutSem;
public:
	AosActMergeFile(const bool flag);
	~AosActMergeFile();
	
	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

	virtual void outPutPost();


private:
	bool getFiles(
				vector<AosNetFileObjPtr> &net_files,
				const AosXmlTagPtr &sdoc, 
				const int crt_level);

	OmnString createFileName(const OmnString &datacol_id, const int level)
	{
		OmnString fname = "mergeFile_";
		fname <<  datacol_id << "_" << level << "_";
		return fname;
	}
};
#endif

