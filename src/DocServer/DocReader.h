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
#if 0
#ifndef AOS_DocServer_DocReader_h
#define AOS_DocServer_DocReader_h

#include "DocServer/Ptrs.h"
#include "API/AosApiG.h"
#include "DocServer/DocBatchReader.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/XmlDoc.h"

class AosDocReader : public OmnThrdShellProc 
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxReadHeader = 20000,
		eDocLength = sizeof(u32),
		eDocIdSize = sizeof(u64),
		eDocidLengthSize = eDocLength + eDocIdSize,
		eMaxRecordDocCacher = 3, 
		eReservedSize = sizeof(int)
	};

private:
	u32										mCrtVirtualId;
	AosBuffPtr								mBuff;
	AosBitmapObjPtr							mBitmap;
	int 									mBitmapIdx;
	u64										mBlockSize;	
	AosRundataPtr							mRundata;
	vector<AosDocBatchReader::FieldDef>		mFields;

public:
	AosDocReader(
			const u32 &vid,
			const OmnString &scanner_id,
			const u64 &block_size,
			const AosBitmapObjPtr &bitmap,
			const int &bitmap_index,
			const vector<AosDocBatchReader::FieldDef> &fields,
			const AosRundataPtr &rdata);
	~AosDocReader();

	virtual bool    run();
	virtual bool    procFinished();
	virtual bool    waitUntilFinished(){return true;}

	u32 getVirtualId () {return mCrtVirtualId;}
	AosBuffPtr getBuff() const;
	int getBitmapIndex() const {return mBitmapIdx;}

private:

	inline bool isGroupedDoc(const u64 &docid)
	{
		return  AosDocType::getDocType(docid) == AosDocType::eGroupedDoc;
	}

	inline u64 convertToGlobalDocid(const u64 &localid, const AosDocType::E &type)
	{
		u64 id = localid * AosGetNumCubes();
		AosDocType::setDocidType(type, id);
		return id + mCrtVirtualId;
	}

	inline u64 convertToLocalDocid(const u64 &docid)
	{
		u64 id = AosXmlDoc::getOwnDocid(docid);
		return id / AosGetNumCubes();
	}

	bool	config(const AosXmlTagPtr &def);

	bool	readNormal(
			const u64 &start_docid,
			u64 &size,
			const u64 snap_id,
			bool &finish);

	bool	batchGetDocs();

	bool	readGroupedDoc(
			const u64 &start_docid,
			u64 &blocksize,
			bool &finish);

	void 	setBitmapIdx(const int &bitmap_idx);

	void	loopData(
			const u64 &start_docid,
			const u32 &sizeid,
			const AosBuffPtr &buff, 
			const int num_docs,
			const int record_len,
			bool &finish,
			u64 &blocksize);

	bool	createNormalDocRecord(
			const AosXmlTagPtr &doc,
			const u64 &docsize,
			u64 &blocksize);

	bool	createFixedLengthRecord(
			const u64 &docid,
			const int &record_size,
			const char *data,
			const u64 &offset,
			u64 &blocksize);

	u32 	getU32Value(
			const AosXmlTagPtr &doc,
			const AosDocBatchReader::FieldDef &field); 

	u64 	getU64Value(
			const AosXmlTagPtr &doc,
			const AosDocBatchReader::FieldDef &field); 

	int64_t getInt64Value(
			const AosXmlTagPtr &doc,
			const AosDocBatchReader::FieldDef &field); 

	OmnString getStringValue(
			const AosXmlTagPtr &doc,
			const AosDocBatchReader::FieldDef &field); 

	bool	procOneField(
			const AosXmlTagPtr &doc,
			const AosValueRslt &valueRslt,
			const AosDocBatchReader::FieldDef &field,
			int &data_len,
			const AosRundataPtr &rdata);

	inline bool isBlockFinished(const u64 &blocksize, const int &record_size)
	{
		if (blocksize + eDocidLengthSize + eReservedSize + record_size > mBlockSize)
		{
			return true;
		}
		return false;
	}
};
#endif

#endif

