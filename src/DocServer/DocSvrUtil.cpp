////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/09/22	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocServer/DocSvrUtil.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "StorageEngine/SengineGroupedDoc.h"


AosDocSvrUtil::AosDocSvrUtil()
{
}

AosDocSvrUtil::~AosDocSvrUtil()
{
}


bool
AosDocSvrUtil::getDocs(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff,
		const u32 num_docids,
		const AosBuffPtr &docids_buff,
		map<u64, int> &sizeid_len,
		u64 &read_end_docid,
		u64 snap_id)
{
	u32 read_num = 0;
	bool hasfield = docids_buff->getU32(0);
	AosBuffPtr field_b;
	u32 field_num = 0;
	vector<AosQrUtil::FieldDef> fields;
	if(hasfield)
	{
		u64 size = docids_buff->getU64(0);
		field_num = docids_buff->getU32(0);
		field_b = docids_buff->getBuff(size, false AosMemoryCheckerArgs);
		for (u32 i = 0; i < field_num; i++)
		{
			AosQrUtil::FieldDef field;
			field.serializeFromBuff(field_b);
			fields.push_back(field);
		}
	}
	

	int64_t docididx = docids_buff->getCrtIdx(); 	
	while(read_num < num_docids)
	{
		docids_buff->setCrtIdx(docididx + read_num * sizeof(u64));
		u64 docid = docids_buff->getU64(0);
		//if (isGroupedDoc(docid))
		//{
			//read fixed docs
		//}
		AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(docid, snap_id, rdata);
		if (!doc)
		{
			OmnScreen << "missing docid:" << docid << endl;
		}
		else
		{
			int docsize = doc->getDocsize();

			setRecord(rdata, hasfield, fields, buff, doc, docid, docsize);

			read_end_docid = docid;
		}
		read_num ++;
	}
	return true;

	/*
	u32 read_num = 0;
	while(read_num < num_docids)
	{
		docids_buff->setCrtIdx(read_num * sizeof(u64));
		u64 docid = docids_buff->getU64(0);
		aos_assert_r(docid, false);

		bool finish = false;
		if (isGroupedDoc(docid))
		{
			readFixedDoc(rdata, buff, docids_buff, 
					num_docids, sizeid_len, read_num, 
					read_end_docid, finish);
		}
		else
		{
			readDoc(rdata, buff, docids_buff, num_docids, read_num, read_end_docid, finish);
		}
		if (finish) break;
	}
	*/
}


AosBuffPtr
AosDocSvrUtil::readDocsFromFile(
		const AosRundataPtr &rdata,
		const u64 start_docid,
		const u32 &vid,
		int &num_docs_to_read)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, 0);

	AosDocFileMgrObjPtr doc_mgr;
	doc_mgr = AosDocSvr::getSelf()->getGroupedDocFileMgr(
		vid, false, rdata);
	aos_assert_r(doc_mgr, 0);

	AosBuffPtr doc_buff = engine->readFixedDocsFromFile(
			start_docid, num_docs_to_read, doc_mgr, rdata);
	aos_assert_r(doc_buff, 0);
	return doc_buff;
}


bool
AosDocSvrUtil::readFixedDoc(
		const AosRundataPtr &rdata,
		const AosBuffPtr &data_buff,
		const AosBuffPtr &docids_buff, 
		const u32 num_docids,
		map<u64, int> sizeid_len,
		u32 &read_num,
		u64 &read_end_docid,
		bool &finish)
{
	docids_buff->setCrtIdx(read_num * sizeof(u64));
	u64 start_docid = docids_buff->getU64(0);

	u32 sizeid = AosGetSizeIdByDocid(start_docid);
	aos_assert_r(sizeid_len.count(sizeid) != 0, false);

	int record_len = sizeid_len[sizeid];
	int num_docs_to_read = (data_buff->buffLen() - data_buff->dataLen()) / (record_len + sizeof(u32) + sizeof(u64));
	if (num_docs_to_read == 0) 
	{
		finish = true;
		return true;
	}
	
	u32 vid = AosGetCubeId(AosXmlDoc::getOwnDocid(start_docid));
	AosBuffPtr doc_buff = readDocsFromFile(rdata, start_docid, vid, num_docs_to_read);
	aos_assert_r(doc_buff, false);
	if (doc_buff->dataLen() <= 0)
	{
		OmnScreen << "Failed to read file: " << start_docid << endl; 
		return true;
	}
	
	u64 start_locid = AosSengineGroupedDoc::getLocalId(start_docid);	
	u64 end_locid = start_locid + num_docs_to_read -1;
	while(read_num < num_docids)
	{
		docids_buff->setCrtIdx(read_num * sizeof(u64));
		u64 docid = docids_buff->getU64(0);
		aos_assert_r(docid && vid == (u32)AosGetCubeId(AosXmlDoc::getOwnDocid(docid)), false);

		u64 loc_id = AosSengineGroupedDoc::getLocalId(docid);
		if (AosGetSizeIdByDocid(docid) != sizeid || loc_id > end_locid)  
		{
			read_num --;
			break;
		}

		u64 offset = loc_id - start_locid * record_len;
		char *data = doc_buff->data();
		data_buff->setU32(record_len);
		data_buff->setU64(docid);
		data_buff->setBuff(&data[offset], record_len);
		read_end_docid = docid;
		read_num ++;
	}
	return true;
}


bool
AosDocSvrUtil::setRecord(
		const AosRundataPtr &rdata,
		const bool &hasfield,
		const vector<AosQrUtil::FieldDef> &fields,
		const AosBuffPtr &data_buff,
		const AosXmlTagPtr &doc,
		const u64 &docid,
		const u64 &docsize)
{
	aos_assert_r(doc, false);

	if (!hasfield)
	{
		data_buff->setU64(docid);
		data_buff->setU32(docsize);
		data_buff->setBuff(doc->toString().data(), docsize);
		return true;
	}


	int64_t field_len_idx = data_buff->getCrtIdx();
	data_buff->setU64(docid);
	data_buff->setU32(0);
	int data_len = 0;
	for (u32 i = 0; i < fields.size(); i++)
	{
		OmnString v;
		AosQrUtil::procOneField(rdata, doc, fields[i], v);
		data_buff->setOmnStr(v);
		data_len += v.length() + sizeof(int);
	}

	// set fields len
	int64_t crt_idx = data_buff->getCrtIdx();
	data_buff->setCrtIdx(field_len_idx);
	data_buff->setU32(data_len);
	data_buff->setCrtIdx(crt_idx);
	return true;
}
