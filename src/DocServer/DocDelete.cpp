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
// 11/21/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocServer/DocDelete.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageEngine/SizeIdMgr.h"
#include "Thread/Mutex.h"  
#include "Thread/CondVar.h"
#include "Thread/Thread.h" 
#include "Util/BuffArray.h"
#include "XmlUtil/XmlDoc.h"

AosDocDelete::AosDocDelete(
			const int &vid,
			const OmnString &scanner_id,
			const AosBitmapObjPtr &bitmap,
			const int &bitmap_index,
			const AosRundataPtr &rdata)
:
OmnThrdShellProc("docbatchdelete"),
mCrtVirtualId(vid),
mBitmap(bitmap),
mBitmapIdx(bitmap_index)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
}

AosDocDelete::~AosDocDelete()
{
}


bool
AosDocDelete::config(const AosXmlTagPtr &def)
{
	return true;
}


void
AosDocDelete::setBitmapIdx(const int &bitmap_idx)
{
	mBitmap->reset();
	int idx = 0;
	u64 value = 0;
	while (idx ++ < bitmap_idx)
	{
		mBitmap->nextDocid(value);
	}
}


bool
AosDocDelete::procFinished()
{
	return true;
}


bool
AosDocDelete::run()
{
	return batchDeleteDocs();
}

bool
AosDocDelete::batchDeleteDocs()
{
	aos_assert_r(mRundata, false);	
	bool rslt = false;
	while (1)
	{
		u64 docid = 0;
		if(!mBitmap->nextDocid(docid))
		{
			mBitmapIdx = -1;
			break;
		}
		mBitmapIdx ++;
		if (isGroupedDoc(docid))
		{
			rslt = deleteGroupedDoc(docid);
			aos_assert_r(rslt, false);
		}
		else
		{
			rslt = deleteNormal(docid);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


bool
AosDocDelete::deleteGroupedDoc(const u64 &start_docid)
{
	u32 sizeid = AosGetSizeIdByDocid(start_docid);
	int record_len = AosSizeIdMgr::getSelf()->getDataRecordLenBySizeId(
						mRundata->getSiteid(), sizeid, mRundata);
	aos_assert_r(record_len >0, false);

	AosDocFileMgrObjPtr docfilemgr;
	//docfilemgr = AosDocSvr::getSelf()->getDocFileMgrBySizeid(
	//			mCrtVirtualId, sizeid, false, mRundata);
	docfilemgr = AosDocSvr::getSelf()->getGroupedDocFileMgr(
				mCrtVirtualId, false, mRundata);

	u64 max_file_size = AosStorageEngineObj::getGroupedDocGroupSize();
	aos_assert_rr(max_file_size > 0, mRundata, 0);

	u32 vnum = (u32)AosGetNumCubes();
	u64 local_id = AosGetLocalIdByDocid(start_docid) / vnum;
	u32 crt_seqno, seqno;
	u64 offset, crt_offset;
	u32 crt_local_id;
	bool rslt = docfilemgr->getPosition(local_id, (u32)record_len, seqno, offset, max_file_size);
	aos_assert_rr(rslt, mRundata, 0);

	u64 docid;
	int num_docs = 1;
OmnScreen <<"vir_id: "<< mCrtVirtualId <<" , docid :" << start_docid <<"  , locaid : " 
	<< local_id <<", crt_seqno :" << seqno << " , crt_offset :" << offset << " , num_docs :" << num_docs << endl;
	while (mBitmap->nextDocid(docid))
	{
		mBitmapIdx ++;
		if (AosGetSizeIdByDocid(docid) != sizeid)
		{
			setBitmapIdx(--mBitmapIdx);
			break;
		}

		// 2. Calculate the location
		crt_local_id = AosGetLocalIdByDocid(docid) / vnum;
		rslt = docfilemgr->getPosition(crt_local_id, (u32)record_len, crt_seqno, crt_offset, max_file_size);
		aos_assert_rr(rslt, mRundata, 0);

		if (crt_seqno != seqno || crt_local_id != local_id +1)
		{
			setBitmapIdx(--mBitmapIdx);
			break;
		}

		local_id = crt_local_id;
		num_docs ++;
//OmnScreen <<"docid : " << docid <<" , locaid : " << local_id <<", crt_seqno :" << crt_seqno << " , crt_offset :" << crt_offset << " , num_docs :" << num_docs << endl;
	}
	
	//Linda, 2013/06/14
	OmnNotImplementedYet;
	return false;
	
	//AosStorageEngineObjPtr engine;
	//engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	//aos_assert_r(engine, false);
	//rslt = engine->deleteDocsFromFile(sizeid, num_docs, record_len, seqno, offset, docfilemgr, mRundata);
	//aos_assert_r(rslt, false);
	//return rslt;
}


bool
AosDocDelete::deleteNormal(const u64 &start_docid)
{
	OmnNotImplementedYet;
	return false;
}
