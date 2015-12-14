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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocServer_DocDelete_h
#define AOS_DocServer_DocDelete_h

#include "DocServer/Ptrs.h"
#include "API/AosApiG.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/XmlDoc.h"

class AosDocDelete : public OmnThrdShellProc 
{
	OmnDefineRCObject;

private:
	int						mCrtVirtualId;
	AosBitmapObjPtr 		mBitmap;
	int 					mBitmapIdx;
	AosRundataPtr			mRundata;

public:
	AosDocDelete(
			const int &vid,
			const OmnString &scanner_id,
			const AosBitmapObjPtr &bitmap,
			const int &bitmap_index,
			const AosRundataPtr &rdata);
	~AosDocDelete();

	virtual bool    run();
	virtual bool    procFinished();
	virtual bool    waitUntilFinished(){return true;}

	int getVirtualId () {return mCrtVirtualId;}
	int getBitmapIndex() const {return mBitmapIdx;}

private:

	inline bool isGroupedDoc(const u64 &docid)
	{
		return  AosDocType::getDocType(docid) == AosDocType::eGroupedDoc;
	}

	bool	config(const AosXmlTagPtr &def);

	void 	setBitmapIdx(const int &bitmap_idx);

	bool 	batchDeleteDocs();

	bool 	deleteGroupedDoc(const u64 &start_docid);

	bool	deleteNormal(const u64 &start_docid);
};
#endif


