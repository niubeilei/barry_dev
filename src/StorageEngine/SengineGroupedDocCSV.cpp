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
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/SengineGroupedDoc.h"

#include "API/AosApiG.h"
#include "DfmUtil/DfmGroupedDoc.h"
#include "SEInterfaces/DocSvrObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageEngine/SengineCSVDocReq.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util1/MemMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


bool
AosSengineGroupedDoc::batchInsertCSVDocPriv(
		const AosSengineDocReqObjPtr& r,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(r, false);

	AosSengineCSVDocReq * req = dynamic_cast<AosSengineCSVDocReq *>(r.getPtr());
	aos_assert_r(req, false);

	u64 snap_id = req->mSnapId;
	u64 sizeid = req->mSizeid;
	u64 num_docs = req->mNumDocs;
	int record_size = req->mRecordSize;
	u64 data_len = req->mDataLen;
	AosBuffPtr b = req->mBuff;

	aos_assert_r(num_docs > 0 && record_size > 0 && data_len > 0 && b, false);
	b->reset();
	AosBuffPtr buff = b->getBuff(data_len, false AosMemoryCheckerArgs);
	aos_assert_r(buff, false);

	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);

	bool need_sort = false;
	u64 local_docid = 0, crt_docid = 0;
	u64 offset = 0, crt_idx = 0;
	u64 groupid = 0, crt_groupid = 0;
	char * doc = 0;
	int len = 0;

	AosBuffPtr doc_buff;
	u64 * offsets = 0;

	for (u64 i = 0; i < num_docs; i++)
	{
		local_docid = b->getU64(0);
		offset = b->getU64(0);
		aos_assert_r(local_docid, false);

		buff->setCrtIdx(offset);
		crt_docid = buff->getU64(0);
		aos_assert_r(local_docid == crt_docid, false);

		len = buff->getInt(0);
		aos_assert_r(len > 0, false);

		offset = buff->getCrtIdx();
		aos_assert_r(buff->dataLen() >= (int)offset + len, false);

		doc = &buff->data()[offset];

		rslt = getGroupid(sizeid, local_docid, max_docs_per_group, groupid);
		aos_assert_r(rslt, false);

		if (groupid != crt_groupid)
		{
			if (crt_groupid > 0)
			{
				aos_assert_r(doc_buff, false);
				rslt = saveGroupDocsCSV(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, need_sort, rdata);
				aos_assert_r(rslt, false);
			}

			crt_groupid = groupid;
			rslt = readGroupDocsCSV(docfilemgr, snap_id, crt_groupid, max_docs_per_group, doc_buff, rdata);
			aos_assert_r(rslt && doc_buff, false);

			need_sort = false;
			doc_buff->setCrtIdx(doc_buff->dataLen());
		}

		offsets = (u64 *)doc_buff->data();
		crt_idx = local_docid % max_docs_per_group;
		if (offsets[crt_idx] > 0)
		{
			need_sort = true;
		}

		offsets[crt_idx] = doc_buff->getCrtIdx();
		doc_buff->setInt(len);
		doc_buff->setBuff(doc, len);
	}	

	rslt = saveGroupDocsCSV(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, need_sort, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosSengineGroupedDoc::batchDeleteCSVDocPriv(
		const AosSengineDocReqObjPtr& r,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(r, false);

	AosSengineCSVDocReq * req = dynamic_cast<AosSengineCSVDocReq *>(r.getPtr());
	aos_assert_r(req, false);

	u64 snap_id = req->mSnapId;
	u64 sizeid = req->mSizeid;
	u64 num_docs = req->mNumDocs;
	int record_size = req->mRecordSize;
	u64 data_len = req->mDataLen;
	AosBuffPtr b = req->mBuff;

	aos_assert_r(num_docs > 0 && record_size > 0 && data_len > 0 && b, false);
	aos_assert_r(b->dataLen() == (i64)(sizeof(u64) * num_docs), false);
	aos_assert_r(b->dataLen() == (i64)data_len, false);
	b->reset();

	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);

	bool need_sort = false;
	u64 local_docid = 0;
	u64 crt_idx = 0;
	u64 groupid = 0, crt_groupid = 0;

	AosBuffPtr doc_buff;
	u64 * offsets = 0;

	for (u64 i = 0; i < num_docs; i++)
	{
		local_docid = b->getU64(0);
		aos_assert_r(local_docid, false);

		rslt = getGroupid(sizeid, local_docid, max_docs_per_group, groupid);
		aos_assert_r(rslt, false);

		if (groupid != crt_groupid)
		{
			if (crt_groupid > 0)
			{
				aos_assert_r(doc_buff, false);
				rslt = saveGroupDocsCSV(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, need_sort, rdata);
				aos_assert_r(rslt, false);
			}

			crt_groupid = groupid;
			rslt = readGroupDocsCSV(docfilemgr, snap_id, crt_groupid, max_docs_per_group, doc_buff, rdata);
			aos_assert_r(rslt && doc_buff, false);

			need_sort = false;
		}

		offsets = (u64 *)doc_buff->data();
		crt_idx = local_docid % max_docs_per_group;
		if (offsets[crt_idx] > 0)
		{
			need_sort = true;
		}

		offsets[crt_idx] = 0;
	}

	rslt = saveGroupDocsCSV(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, need_sort, rdata);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosSengineGroupedDoc::batchUpdateCSVDocPriv(
		const AosSengineDocReqObjPtr& r,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosSengineGroupedDoc::insertCSVDoc(
		const u64 &snap_id,
		const u64 &sizeid,
		const int record_size,
		const AosBuffPtr &buff,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}



bool
AosSengineGroupedDoc::saveGroupDocsCSV(
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const u64 &snap_id,
		const u32 &groupid, 
		const AosBuffPtr &doc_buff,
		const bool need_sort,
		const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, false);
	
	//AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, groupid, false);
	AosDfmDocPtr p_doc = readDocPriv(rdata, docfilemgr, snap_id, groupid, false);
	if (!p_doc) p_doc = OmnNew AosDfmGroupedDoc(siteid, groupid);

	bool is_compress = AosDocSvrObj::getDocSvr()->getGroupedDocWithComp();
is_compress = false;
	p_doc->setNeedCompress(is_compress);
	p_doc->setBodyBuff(doc_buff);

	//bool rslt = docfilemgr->saveDoc(rdata, trans_id, p_doc);
	bool rslt = saveDocPriv(rdata, docfilemgr, trans_id, snap_id, p_doc);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosSengineGroupedDoc::readGroupDocsCSV(
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const u32 &groupid,
		const u64 &max_docs_per_group,
		AosBuffPtr &doc_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docfilemgr, false);

	u64 offset_map_size = sizeof(u64) * max_docs_per_group;
	//AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, groupid);
	AosDfmDocPtr p_doc = readDocPriv(rdata, docfilemgr, snap_id, groupid, true);
	if(!p_doc)
	{
		doc_buff = OmnNew AosBuff(offset_map_size + 20 AosMemoryCheckerArgs);
		memset(doc_buff->data(), 0, offset_map_size);
		doc_buff->setDataLen(offset_map_size);
		return true;
	}

	doc_buff = p_doc->getBodyBuff();
	aos_assert_r(doc_buff && doc_buff->dataLen() >= (int64_t)offset_map_size, false);

	return true;
}


bool
AosSengineGroupedDoc::readRecordCSV(
		const u64 &sizeid,
		const u64 &local_docid,
		char **data,
		int &len,
		const int record_size,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);

	u64 crt_groupid;
	rslt = getGroupid(sizeid, local_docid, max_docs_per_group, crt_groupid);
	aos_assert_r(rslt, false);
	
	AosBuffPtr doc_buff;
	rslt = readGroupDocsCSV(docfilemgr, 0, crt_groupid, max_docs_per_group, doc_buff, rdata);
	aos_assert_r(rslt && doc_buff && (u64)doc_buff->dataLen() > max_docs_per_group * sizeof(u64), false);

	u64 crt_idx = local_docid % max_docs_per_group;
	u64 *offsets = (u64 *)doc_buff->data();
	u64 offset = offsets[crt_idx];
	aos_assert_r(offset > 0 && (u64)doc_buff->dataLen() >= offset + sizeof(int), false);

	doc_buff->setCrtIdx(offset);
	len = doc_buff->getInt(-1);
	aos_assert_r(len > 0 && (u64)doc_buff->dataLen() >= offset + sizeof(int) + len, false);

	offset = doc_buff->getCrtIdx();
	aos_assert_r(doc_buff->dataLen() >= (int)offset + len, false);

	char * d = &doc_buff->data()[offset];
	*data = OmnMemMgrSelf->allocate(len + 1, __FILE__, __LINE__);
	memcpy(*data, d, len);
	(*data)[len] = 0;

	return true;
}


bool
AosSengineGroupedDoc::readRecordsCSV(
		const u64 &sizeid,
		const u64 &local_docid,
		const int &record_size,
		const AosBuffPtr &buff, 
		const u32 max,
		int &read_num_docs,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);

	u64 crt_groupid;
	rslt = getGroupid(sizeid, local_docid, max_docs_per_group, crt_groupid);
	aos_assert_r(rslt, false);
	
	AosBuffPtr doc_buff;
	rslt = readGroupDocsCSV(docfilemgr, 0, crt_groupid, max_docs_per_group, doc_buff, rdata);
	aos_assert_r(rslt && doc_buff && (u64)doc_buff->dataLen() > max_docs_per_group * sizeof(u64), false);

	u64 crt_idx = local_docid % max_docs_per_group;
	u64 *offsets = (u64 *)doc_buff->data();
	u64 offset = offsets[crt_idx];
	int len = 0;
	char * doc = 0;
	read_num_docs = 0;

	while (true)
	{
		aos_assert_r((u64)doc_buff->dataLen() >= offset + sizeof(int), false);

		doc_buff->setCrtIdx(offset);
		len = doc_buff->getInt(-1);
		aos_assert_r(len > 0 && (u64)doc_buff->dataLen() >= offset + sizeof(int) + len, false);

		if (buff->dataLen() + sizeof(int) + len > max)
		{
			break;
		}

		offset = doc_buff->getCrtIdx();
		aos_assert_r(doc_buff->dataLen() >= (int)offset + len, false);
		doc = &doc_buff->data()[offset];

		buff->setInt(len);
		buff->setBuff(doc, len);
		read_num_docs++;

		crt_idx++;
		offset = offsets[crt_idx];
	}

	return true;
}


AosXmlTagPtr 
AosSengineGroupedDoc::readCSVDocFromFile(
		const u64 &docid,
		const int record_size,
		const AosDocFileMgrObjPtr &docfilemgr, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, 0);
	aos_assert_r(docfilemgr, 0);
	aos_assert_r(record_size > 0, 0);

	u64 sizeid = AosGetSizeIdByDocid(docid);
	u64 local_docid = getLocalId(docid);

	char * mem = 0;
	int len = 0;
	bool rslt = readRecordCSV(sizeid, local_docid,
		&mem, len, record_size, docfilemgr, rdata); 
	if (!rslt || !mem) 
	{
		OmnAlarm << "failed reading file: " << docid << ":" << enderr;
		if (mem) OmnMemMgrSelf->release(mem, __FILE__, __LINE__);
		return 0;
	}

	OmnString docstr;
	docstr << "<doc "
		   << AOSTAG_DOCID << "=\"" << docid << "\" "
		   << AOSTAG_SITEID << "=\"" << rdata->getSiteid() << "\"><![CDATA[";
	docstr.append(mem, len);
	docstr << "]]></doc>";

	OmnMemMgrSelf->release(mem, __FILE__, __LINE__);

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	
	return doc;
}


AosBuffPtr
AosSengineGroupedDoc::readCSVDocsFromFile(
		const AosRundataPtr &rdata,
		const vector<u64> &docids,
		const int record_size)
{
	// felicia, 2014/05/08
	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	aos_assert_r(max_group_size > 0, 0);
	
	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, 0);

	//1. build group_map
	u64 crt_groupid = 0;
	u64 groupid = 0;
	u64 sizeid = 0;
	u64 local_docid = 0;
	u32 num_docs = 0;
	u32 start_idx = 0;
	
	u32 max = record_size * docids.size();
	AosBuffPtr buff = OmnNew AosBuff(max AosMemoryCheckerArgs);

	for (u32 i=0; i<docids.size(); i++, num_docs++)
	{
		sizeid = AosGetSizeIdByDocid(docids[i]);
		local_docid = getLocalId(docids[i]);
		rslt = getGroupid(sizeid, local_docid, max_docs_per_group, groupid);
		aos_assert_r(rslt, 0);

		if(groupid != crt_groupid)
		{
			if (num_docs > 0)
			{
				rslt = batchReadCSVDocs(record_size, start_idx, num_docs, docids, crt_groupid, buff, rdata);
				aos_assert_r(rslt, 0);
			}
			crt_groupid = groupid;
			start_idx = i;
			num_docs = 0;
		}
	}
	
	if (num_docs > 0)
	{
		rslt = batchReadCSVDocs(record_size, start_idx, num_docs, docids, crt_groupid, buff, rdata);
		aos_assert_r(rslt, 0);
	}

	return buff;
}


bool
AosSengineGroupedDoc::batchReadCSVDocs(
		const int record_size, 
		const u32 start_idx, 
		const u32 num_docs, 
		const vector<u64> &docids, 
		const u64 crt_groupid,
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(start_idx < docids.size(), false);
	aos_assert_r(start_idx + num_docs <= docids.size(), false);
	
	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);

	u32 vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docids[start_idx]));
	AosDocFileMgrObjPtr doc_mgr = AosDocSvrObj::getDocSvr()->getGroupedDocFileMgr(
		vid, false, rdata);                                
	aos_assert_r(doc_mgr, false);
	
	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	AosBuffPtr doc_buff = OmnNew AosBuff(max_group_size AosMemoryCheckerArgs);
	rslt = readGroupDocsCSV(doc_mgr, 0, crt_groupid, max_docs_per_group, doc_buff, rdata);
	aos_assert_r(rslt && doc_buff && (u64)doc_buff->dataLen() > max_docs_per_group * sizeof(u64), false);

	u64 offset_map_size = sizeof(u64) * max_docs_per_group;
	u64 local_docid = 0;
	u64 crt_idx = 0;
	u64 *offsets = (u64 *)doc_buff->data();
	u64 offset = 0;
	int len = 0;
	char * doc = 0;
	for (u32 i=start_idx; i<start_idx + num_docs; i++)
	{
		local_docid = getLocalId(docids[i]);
		crt_idx = local_docid % max_docs_per_group;
		offset = offsets[crt_idx];

		aos_assert_r(offset >= offset_map_size, false);
		aos_assert_r((u64)doc_buff->dataLen() >= offset + sizeof(int), false);

		doc_buff->setCrtIdx(offset);
		len = doc_buff->getInt(-1);
		aos_assert_r(len > 0 && (u64)doc_buff->dataLen() >= offset + sizeof(int) + len, false);

		offset = doc_buff->getCrtIdx();
		aos_assert_r(doc_buff->dataLen() >= (int)offset + len, false);
		doc = &doc_buff->data()[offset];
	
		buff->setU64(docids[i]);
		buff->setU32(len);
		buff->setBuff(doc, len);
	}

	return true;
}


AosBuffPtr
AosSengineGroupedDoc::readCSVDocsFromFile(
		const u64 &docid, 
		int &num_docs,
		const AosDocFileMgrObjPtr &docfilemgr, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, 0);
	aos_assert_r(docfilemgr, 0);

	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	aos_assert_r(max_group_size > 0, 0);

	// 1. Retrieve Data Record
	AosDataRecordObjPtr record = AosSizeIdMgr::getSelf()->getDataRecordByDocid(
			rdata->getSiteid(), docid, rdata);
	if (!record)
	{
		// Did not find the data record. This is an error.
		AosSetErrorU(rdata, "no_data_record_001") << ": " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	int record_size = record->getEstimateRecordLen();
	aos_assert_r(record_size > 0, 0);

	u64 sizeid = AosGetSizeIdByDocid(docid);
	u64 local_id = getLocalId(docid);

	// 2. Read from file
	u32 max = record_size * num_docs;
	if(max > max_group_size)
	{
		max = max_group_size;
	}

	AosBuffPtr buff = OmnNew AosBuff(max AosMemoryCheckerArgs);
	bool rslt = readRecordsCSV(sizeid, local_id,
		record_size, buff, max, num_docs, docfilemgr, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_read_file") << ": " << docid << enderr;
		buff->setDataLen(0);
		return buff;
	}
	return buff;
}

