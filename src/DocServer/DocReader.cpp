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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/DocReader.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "StorageEngine/SizeIdMgr.h"
#include "Thread/Mutex.h"  
#include "Thread/CondVar.h"
#include "Thread/Thread.h" 
#include "Util/BuffArray.h"
#include "XmlUtil/XmlDoc.h"

AosDocReader::AosDocReader(
			const u32 &vid,
			const OmnString &scanner_id,
			const u64 &block_size,
			const AosBitmapObjPtr &bitmap,
			const int &bitmap_index,
			const vector<AosDocBatchReader::FieldDef> &fields,
			const AosRundataPtr &rdata)
:
OmnThrdShellProc("docbatchreader"),
mCrtVirtualId(vid),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mBitmap(bitmap),
mBitmapIdx(bitmap_index),
mBlockSize(block_size),
mFields(fields)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
}

AosDocReader::~AosDocReader()
{
}


bool
AosDocReader::config(const AosXmlTagPtr &def)
{
	return true;
}


void
AosDocReader::setBitmapIdx(const int &bitmap_idx)
{
	mBitmap->reset();
	int idx = 0;
	while (idx ++ < bitmap_idx)
	{
		mBitmap->nextDocid();
	}
}


bool
AosDocReader::readNormal(
		const u64 &start_docid,
		u64 &blocksize,
		const u64 snap_id,
		bool &finish)
{
	// This function reads in a number of docs starting from 'start_docid'. 
	// It reads maximum 'mBlockSize' number of bytes. Since this is a 
	// normal length docid. The results are saved in mBuff. mBuff format
	// is:
	// 		doc_length		(4 bytes)
	// 		docid			(8 bytes)
	// 		doc contents	(variable)
	
	finish = false;
	u64 docid = start_docid;
	do
	{
		if (AosDocType::getDocType(docid) == AosDocType::eGroupedDoc) 
		{ 
			setBitmapIdx( -- mBitmapIdx);
			break;
		}
	
		AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(docid, snap_id, mRundata);
		if (!doc)
		{
			OmnScreen << "missing docid:" << docid << endl;
		}
		else
		{
			aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);

			u64 docsize = doc->getDocsize();
			finish = isBlockFinished(blocksize, docsize);
			if (finish) break;
			createNormalDocRecord(doc, docsize, blocksize);
		}
		mBitmapIdx ++;

	}while ((docid = mBitmap->nextDocid()));
	
	return true;
	//	aos_assert_r(doc && doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
	//
	//	mBuff->setU32(docsize);
	//	mBuff->setU64(docid);
	//	mBuff->setBuff(doc->toString().data(), docsize);
	//
	//	size += docsize + sizeof(u64) + sizeof(u32);
	//	mBitmapIdx ++;
	//
	//}while ((docid = mBitmap->nextDocid()));
	//
	//if (docid == 0) mBitmapIdx = -1; 
	//return true;


	//finish = false;
	//u32 header_size = AosXmlDoc::getHeaderSize();
	//aos_assert_r(header_size > 0, false);

	//AosDocType::E type = AosDocType::getDocType(start_docid);
	//u64 start_local_docid = convertToLocalDocid(start_docid);
	
	//int num_headers_to_read = eMaxReadHeader / (header_size + sizeof(u32) + sizeof(u64));
	//int num_headers = num_headers_to_read;

	//AosDocFileMgrObjPtr docfilemgr;
	//docfilemgr = AosDocSvr::getSelf()->getDocFileMgrByVid(
	//		mCrtVirtualId, AosModuleId::eDoc, mRundata);
	//aos_assert_r(docfilemgr, false);

	//const int buffsize = header_size* num_headers;
	//AosBuff buff(buffsize +10 AosMemoryCheckerArgs);
	
	//bool rslt = AosXmlDoc::readHeaders(docfilemgr, start_local_docid, num_headers, buff, mRundata);
	//aos_assert_r(rslt, false);
	//if (buff.dataLen() <= 0)
	//{
	//	OmnAlarm << "Failed to read file: " << start_docid << enderr; 
	//	return true;
	//}
	//aos_assert_r(num_headers && num_headers <= num_headers_to_read, false); 

	//u64 end_local_docid = start_local_docid + num_headers - 1;
	//u64 end_docid = convertToGlobalDocid(end_local_docid, type);
	//buff.reset();
	//u32 seqno, seqSys;
	//u64 offset, docsize, offSys, sizeSys, compressedSize;
	//u64 docid = start_docid;
	//do
	//{
	//	if (docid > end_docid || AosDocType::getDocType(docid) == AosDocType::eGroupedDoc) 
	//	{ 
	//		setBitmapIdx( -- mBitmapIdx);
	//		break;
	//	}
	//
	//	u64 local_docid = convertToLocalDocid(docid);
	//	u64 crt_idx = (local_docid - start_local_docid) * header_size; 
	//	buff.setCrtIdx(crt_idx);
	//
	//	buff.getInt(0);	
	//	rslt= AosXmlDoc::readFromBuff(buff, local_docid, seqno, offset, docsize, seqSys, offSys, sizeSys, compressedSize, false, mRundata); 
	//	aos_assert_r(rslt, false);
	//
	//	if (size + sizeof(u32) + sizeof(u64) + docsize > mBlockSize) 
	//	{
	//		setBitmapIdx( -- mBitmapIdx);
	//		finish = true; 
	//		break;
	//	}
	//
	//	AosXmlTagPtr doc = AosXmlDoc::readDoc(docfilemgr, seqno, offset, docsize, compressedSize, mRundata);
	//	aos_assert_r(doc && doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
	//
	//	mBuff->setU32(docsize);
	//	mBuff->setU64(docid);
	//	mBuff->setBuff(doc->toString().data(), docsize);
	//
	//	size += docsize + sizeof(u64) + sizeof(u32);
	//	mBitmapIdx ++;
	//
	//}while ((docid = mBitmap->nextDocid()));
	//
	//if (docid == 0) mBitmapIdx = -1; 
	//return true;
}


bool
AosDocReader::createNormalDocRecord(
		const AosXmlTagPtr &doc,
		const u64 &docsize,
		u64 &blocksize)
{
	// This function reads in a number of docs starting from 'start_docid'. 
	// It reads maximum 'mBlockSize' number of bytes. Since this is a 
	// fixed length docid. The results are saved in mBuff. mBuff format(eXml)
	// is:
	// 		doc_length		(4 bytes)
	// 		docid			(8 bytes)
	// 		doc contents	(variable)
	//
	// mBuff format(eAttr or eText) is:
	//		doc_length		(4 bytes)
	//		docid			(8 bytes)
	//		fname0			(data_type0)
	//		fname1			(data_type1)
	//		...
	//		fname2			(data_type2)
	
	aos_assert_r(doc, false);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	if (mFields.size() == 0 || mFields[0].type == AosDocBatchReader::eXml)
	{
		mBuff->setU32(docsize);
		mBuff->setU64(docid);
		mBuff->setBuff(doc->toString().data(), docsize);
		blocksize += eDocidLengthSize + docsize;
		return true;
	}

	int64_t field_len_idx = mBuff->getCrtIdx();
	mBuff->setU32(0);
	mBuff->setU64(docid);

	int data_len = eDocidLengthSize; 		
	for (u32 i = 0; i < mFields.size(); i++)
	{
		AosDocBatchReader::FieldDef field = mFields[i]; 
		aos_assert_r(field.type == AosDocBatchReader::eAttr || field.type == AosDocBatchReader::eText, false);
		AosValueRslt vv;
		procOneField(doc, vv, field, data_len, mRundata);
	}
	blocksize += data_len;

	// set fields len
	int64_t crt_idx = mBuff->getCrtIdx();
	mBuff->setCrtIdx(field_len_idx);
	mBuff->setU32(data_len);
	mBuff->setCrtIdx(crt_idx);
	return true;
}


bool
AosDocReader::batchGetDocs()
{
	bool rslt = false;
	u64 size = 0;
	bool finish = false;
	aos_assert_r(mBitmap, false);
	aos_assert_r(mBlockSize > 0, false);
	aos_assert_r(mBuff, false);
	aos_assert_r(mRundata, false);
int cout = mBitmapIdx;
	while (size < mBlockSize)
	{
		u64 docid = mBitmap->nextDocid();
		if (docid == 0) 
		{
OmnScreen << "mCrtVirtualId: " << mCrtVirtualId << " , Block Num Docs: " << mBitmapIdx - cout << endl;
			mBitmapIdx = -1;
			break;
		}
		mBitmapIdx ++;	
		if (isGroupedDoc(docid))
		{
			rslt = readGroupedDoc(docid, size, finish);
			aos_assert_r(rslt, false);
		}
		else
		{
			rslt = readNormal(docid, size, finish);
			aos_assert_r(rslt, false);
		}

		if (finish) 
		{
			setBitmapIdx( -- mBitmapIdx);
			break; 
		}
	}
if (mBitmapIdx >=0) OmnScreen << "mCrtVirtualId: " << mCrtVirtualId << " , Block Num Docs: " << mBitmapIdx - cout << endl;
	return true;
}


bool
AosDocReader::readGroupedDoc(
		const u64 &start_docid, 
		u64 &blocksize,
		bool &finish)
{
	// This function reads in a number of docs starting from 'start_docid'. 
	// It reads maximum 'mBlockSize' number of bytes. Since this is a 
	// fixed length docid. The results are saved in mBuff. mBuff format
	// is:
	// 		doc_length		(4 bytes)
	// 		docid			(8 bytes)
	// 		doc contents	(variable)
	
	finish = false;
	aos_assert_r(blocksize < mBlockSize, false);

	int record_len = AosGetDataRecordLenByDocid(mRundata->getSiteid(), start_docid, mRundata);
	aos_assert_r(record_len >0 && (u32)record_len <= mBlockSize, false);
	
	finish = isBlockFinished(blocksize, record_len);
	if (finish) return true;

	int num_docs_to_read = (mBlockSize - blocksize)/ (record_len + eDocidLengthSize);
	aos_assert_r(num_docs_to_read > 0, false);
	int num_docs = num_docs_to_read;

	u32 sizeid = AosGetSizeIdByDocid(start_docid);
	AosDocFileMgrObjPtr docfilemgr;
	docfilemgr = AosDocSvr::getSelf()->getGroupedDocFileMgr(
				mCrtVirtualId, false, mRundata);
	aos_assert_r(docfilemgr, false);

	AosStorageEngineObjPtr engine;
	engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosBuffPtr buff = engine->readFixedDocsFromFile(start_docid, num_docs, docfilemgr, mRundata);
	aos_assert_r(buff, false);
	if (buff->dataLen() <= 0)
	{
		OmnScreen << "Failed to read file: " << start_docid << endl; 
		return true;
	}
	aos_assert_r(buff && buff->dataLen() > 0, false);
	aos_assert_r(num_docs && num_docs <= num_docs_to_read, false);

	loopData(start_docid, sizeid, buff, num_docs, record_len, finish, blocksize);
	return true;
}


void
AosDocReader::loopData(
		const u64 &start_docid,
		const u32 &sizeid,
		const AosBuffPtr &buff,
		const int num_docs,
		const int record_size,
		bool &finish,
		u64 &blocksize)
{
	AosDocType::E type = AosDocType::getDocType(start_docid);
	u64 start_localid = convertToLocalDocid(start_docid);

	u64 end_localid = start_localid + num_docs -1;
	u64 end_docid = convertToGlobalDocid(end_localid, type);

	const char *data = buff->data();
	u64 docid = start_docid;
	do
	{
		finish = isBlockFinished(blocksize, record_size);
		if (AosGetSizeIdByDocid(docid) != sizeid || docid > end_docid) 
		{ 
			if (AosGetSizeIdByDocid(docid) != sizeid)
			{
				OmnScreen << "old sizeid :" << sizeid  << " , " << record_size 
					<< " , new sizeid :" << AosGetSizeIdByDocid(docid) << endl;
			}
	
			if (!finish) setBitmapIdx( -- mBitmapIdx);
			break;
		}

		if (finish) break;
		u64 local_docid = convertToLocalDocid(docid);
		u64 offset = (local_docid - start_localid) * record_size; 

		createFixedLengthRecord(docid, record_size, data, offset, blocksize);	
		mBitmapIdx ++;
	}while ((docid = mBitmap->nextDocid()));
}


bool
AosDocReader::createFixedLengthRecord(
		const u64 &docid,
		const int &record_size,
		const char *data,
		const u64 &offset,
		u64 &blocksize)
{
	// This function reads in a number of docs starting from 'start_docid'. 
	// It reads maximum 'mBlockSize' number of bytes. Since this is a 
	// fixed length docid. The results are saved in mBuff. mBuff format(eXml)
	// is:
	// 		doc_length		(4 bytes)
	// 		docid			(8 bytes)
	// 		doc contents	(variable)
	//
	// mBuff format(eAttr or eText) is:
	//		doc_length		(4 bytes)
	//		docid			(8 bytes)
	//		fname0			(data_type0)
	//		fname1			(data_type1)
	//		...
	//		fname2			(data_type2)
	
	if (mFields.size() == 0 || mFields[0].type == AosDocBatchReader::eXml)
	{
		mBuff->setU32(record_size);
		mBuff->setU64(docid);
		mBuff->setBuff(&data[offset], record_size);
		blocksize += eDocidLengthSize + record_size;
		return true;
	}
	
	AosDataRecordObjPtr record = AosGetDataRecordByDocid(mRundata->getSiteid(), docid, mRundata); 
	if (!record)
	{
		// Did not find the data record. This is an error.
		AosSetErrorU(mRundata, "no_data_record_001") << ": " << docid;
		OmnAlarm << mRundata->getErrmsg() << enderr;
		return false;
	}

	int64_t field_len_idx = mBuff->getCrtIdx();
	mBuff->setU32(0);
	mBuff->setU64(docid);
	
	int data_len = eDocidLengthSize;
	for (u32 i = 0; i < mFields.size(); i++)
	{
		AosDocBatchReader::FieldDef field = mFields[i]; 
		aos_assert_r(field.type == AosDocBatchReader::eAttr || field.type == AosDocBatchReader::eText, false);
		AosValueRslt valueRslt;
		record->setData((char *)&data[offset], record_size, 0, 0);
		record->getFieldValue(field.fname, valueRslt, true, mRundata.getPtr());
		procOneField(0, valueRslt, field, data_len, mRundata);
	}

	blocksize += data_len;

	// set fields len
	int64_t crt_idx = mBuff->getCrtIdx();
	mBuff->setCrtIdx(field_len_idx);
	mBuff->setU32(data_len);
	mBuff->setCrtIdx(crt_idx);
	return true;
}


AosBuffPtr
AosDocReader::getBuff() const
{
	return mBuff;
}


bool
AosDocReader::procFinished()
{
	return true;
}


bool
AosDocReader::run()
{
	return batchGetDocs();
}


bool
AosDocReader::procOneField(
		const AosXmlTagPtr &doc,
		const AosValueRslt &valueRslt,
		const AosDocBatchReader::FieldDef &field,
		int &data_len,
		const AosRundataPtr &rdata)
{
	switch (field.data_type)
	{
	case AosDataType::eU64:
		 {
			u64 vv = 0;
			if (doc)
			{
				vv = getU64Value(doc, field);
			}
			else
			{
				valueRslt.getU64Value(vv, rdata.getPtr());
			}
			mBuff->setU64(vv); 
			data_len += AosDataType::getValueSize(field.data_type);
		 }
		 break;

	case AosDataType::eU32:
		 {
			u32 vv = 0;
			if (doc)
			{
				vv = getU32Value(doc, field);
			}
			else
			{
				vv = valueRslt.getU32Value(rdata.getPtr());
			}
			mBuff->setU32(vv);
			data_len += AosDataType::getValueSize(field.data_type);
		 }
		 break;

	case AosDataType::eInt64:
		 { 
			int64_t vv = 0;
			if (doc)
			{
				vv = getInt64Value(doc, field);
			}
			else
			{
				vv = valueRslt.getInt64Value(rdata.getPtr());
			}
			mBuff->setI64(vv);
			data_len += AosDataType::getValueSize(field.data_type);
		 }
		 break;

	case AosDataType::eString:
		 {
			OmnString vv;
			if (doc)
			{
				vv = getStringValue(doc, field); 
			}
			else
			{
				vv = valueRslt.getValueStr1();
			}

			mBuff->setOmnStr(vv);
			data_len += vv.length() + sizeof(int);
		 }
		 break;

	default:
		 OmnAlarm << "Incorrec type:" << field.data_type << enderr;
		 break;
	}
	return true;
}


u32 
AosDocReader::getU32Value(
		const AosXmlTagPtr &doc,
		const AosDocBatchReader::FieldDef &field)
{
	u32 value = 0;
	switch(field.type)
	{
	case AosDocBatchReader::eAttr:
		 value = doc->getAttrU32(field.fname, 0);
		 break;

	case AosDocBatchReader::eText:
		 {
			  OmnString vv = doc->getNodeText(field.fname);
			  if (vv != "")
			  {
				value = vv.parseU32(0);
			  }
			  break;
		 }

	default:
		OmnAlarm << "Incorrec type:" << field.type << enderr;
		break;
	}
	return value;
}


u64 
AosDocReader::getU64Value(
		const AosXmlTagPtr &doc,
		const AosDocBatchReader::FieldDef &field)
{
	u64 value = 0;
	switch(field.type)
	{
	case AosDocBatchReader::eAttr:
		 value = doc->getAttrU64(field.fname, 0);
		 break;

	case AosDocBatchReader::eText:
		 {
			  OmnString vv = doc->getNodeText(field.fname);
			  if (vv != "")
			  {
				value = vv.parseU64(0);
			  }
			  break;
		 }

	default:
		OmnAlarm << "Incorrec type:" << field.type << enderr;
		break;
	}
	return value;
}


int64_t
AosDocReader::getInt64Value(
		const AosXmlTagPtr &doc,
		const AosDocBatchReader::FieldDef &field)
{
	int64_t value = 0;
	switch(field.type)
	{
	case AosDocBatchReader::eAttr:
		 value = doc->getAttrInt64(field.fname, 0);
		 break;

	case AosDocBatchReader::eText:
		 {
			  OmnString vv = doc->getNodeText(field.fname);
			  if (vv != "")
			  {
				value = vv.parseInt64(0);
			  }
			  break;
		 }

	default:
		OmnAlarm << "Incorrec type:" << field.type << enderr;
		break;
	}
	return value;
}


OmnString
AosDocReader::getStringValue(
		const AosXmlTagPtr &doc,
		const AosDocBatchReader::FieldDef &field)

{
	OmnString value;
	switch(field.type)
	{
	case AosDocBatchReader::eAttr:
		 value = doc->getAttrStr(field.fname, "");
		 break;

	case AosDocBatchReader::eText:
		 value = doc->getNodeText(field.fname);
		 break;

	default:
		OmnAlarm << "Incorrec type:" << field.type << enderr;
		break;
	}
	return value;
}

#endif
