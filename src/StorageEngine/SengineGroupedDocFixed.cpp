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
#include "StorageEngine/SengineFixedDocReq.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util1/MemMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


bool
AosSengineGroupedDoc::saveBatchFixedDocPriv(
		const AosSengineDocReqObjPtr& r,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(r, false);

	AosSengineFixedDocReq * req = dynamic_cast<AosSengineFixedDocReq *>(r.getPtr());
	aos_assert_r(req, false);

	u64 snap_id = req->mSnapId;
	u64 sizeid = req->mSizeid;
	u64 num_docs = req->mNumDocs;
	int record_size = req->mRecordSize;
	const u64 *docids = (u64 *)req->mDocids->data();
	AosBuffPtr buff = req->mRawData;

	aos_assert_r(num_docs > 0 && record_size > 0 && buff, false);

	/*
	 * Chen Ding, 2013/03/24
	 * There is a 'max_group_size' limit. 'max_group_size' is configurable
	 * and defaults to 4M. Once 'max_group_size' and 'record_size' is 
	 * determined, the number of docs per group is determined. 
	*/
	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);
	const char *data = buff->data();

	char cmp_data[record_size];
	memset(cmp_data, 0, record_size);

	/*
	int crt_idx = 0;
	u64 len = 0;
	u64 crt_array_idx = 0;
	rslt = false;
	u64 crt_groupid = 0;

	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	AosBuffPtr doc_buff = OmnNew AosBuff(max_group_size AosMemoryCheckerArgs);
	memset(doc_buff->data(), 0, max_group_size);
	doc_buff->setDataLen(max_group_size);
	char *data_array = 0;		
	for (u64 start_idx = 0; start_idx < num_docs; start_idx++)
	{
		u64 local_docid = docids[start_idx];
		if (local_docid == 0)
		{
			// Found an invalid docid, which means that it stops the current
			// docid range. If 'data_array' is not empty, it means a docid range
			// has been found. It needs to copy the data into 'data_array'.
			if (data_array)
			{
				// A range is found:
				// 		[crt_
				aos_assert_r(len > 0, false);
				memcpy(&data_array[crt_array_idx*record_size], &data[crt_idx * record_size], len);

				//checkFullDocs(len, cmp_data, data_array, record_size, is_full, rdata);
				rslt = saveGroupDocsFixed(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, rdata);
				aos_assert_r(rslt, false);
			}

			data_array = 0;
			len = 0;
			crt_groupid = 0;
			continue;
		}

		// Chen Ding, 2013/03/24
		// Check whether 'local_docid' is in the current group. 
		u64 groupid = 0;
		rslt = getGroupid(sizeid, local_docid, max_docs_per_group, groupid);
		aos_assert_r(rslt, false);

		// 'local_docid' is not 0. Need to read in its file.
		if (len == 0)
		{
			crt_groupid = groupid;
			rslt = readGroupDocsFixed(docfilemgr, snap_id, crt_groupid, doc_buff, rdata);
			aos_assert_r(rslt, false);
			data_array = (char*)doc_buff->data();

			crt_array_idx = local_docid % max_docs_per_group;
			crt_idx = start_idx;
			len += record_size;
			continue;
		}

		// This means the current docid (i.e., local_docid) is not 0, 
		// and the current block is not empty, then local_docid must 
		// be the previous docid + 1. Otherwise, it is an error.
		if (local_docid != docids[start_idx - 1] + 1)
		{
			OmnAlarm << "docid error" << enderr;
			return false;
		}

		// The current block is not empty. It needs to check the new docid
		// is in the same file (i.e., its seqno == first_seqno). If it is
		// not, it needs to save the current block.
		if(crt_groupid != groupid)
		{
			// The new docid is in a different group. 
			// It needs to save the current group.
			if(len > 0)
			{
				aos_assert_r(data_array, false);
				memcpy(&data_array[crt_array_idx * record_size], &data[crt_idx * record_size], len);
			}
			//checkFullDocs(len, cmp_data, data_array, record_size, is_full, rdata);

			rslt = saveGroupDocsFixed(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, rdata);
			aos_assert_r(rslt, false);

			// 2. Open the new file
			crt_groupid = groupid;
			rslt = readGroupDocsFixed(docfilemgr, snap_id, crt_groupid, doc_buff, rdata);
			aos_assert_r(rslt, false);

			data_array = (char*)doc_buff->data();
			crt_array_idx = local_docid % max_docs_per_group;
			crt_idx = start_idx;
			len = record_size;
			continue;
		}

		len += record_size;
	}	

	if(len > 0)
	{
		// 1 Need to save the current file.
		aos_assert_r(data_array, false);

		memcpy(&data_array[crt_array_idx*record_size], &data[crt_idx * record_size], len);
		//checkFullDocs(len, cmp_data, data_array, record_size, is_full, rdata);

		rslt = saveGroupDocsFixed(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, rdata);
		aos_assert_r(rslt, false);
	}
	*/

	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	AosBuffPtr doc_buff = OmnNew AosBuff(max_group_size AosMemoryCheckerArgs);
	memset(doc_buff->data(), 0, max_group_size);
	doc_buff->setDataLen(max_group_size);	

	u64 local_docid = docids[0];

	u64 crt_groupid = 0;
	rslt = getGroupid(sizeid, local_docid, max_docs_per_group, crt_groupid);
	aos_assert_r(rslt, false);

	rslt = readGroupDocsFixed(docfilemgr, snap_id, crt_groupid, doc_buff, rdata);
	aos_assert_r(rslt, false);
	char *data_array = (char*)doc_buff->data();

	int crt_array_idx = local_docid % max_docs_per_group;
	memcpy(&data_array[crt_array_idx * record_size], data, record_size);

	for (u64 start_idx = 1; start_idx < num_docs; start_idx++)
	{
		u64 local_docid = docids[start_idx];
		if (local_docid == 0)
		{
			continue;
		}

		u64 groupid = 0;
		rslt = getGroupid(sizeid, local_docid, max_docs_per_group, groupid);
		aos_assert_r(rslt, false);

		if (crt_groupid == groupid)
		{
			crt_array_idx = local_docid % max_docs_per_group;
			memcpy(&data_array[crt_array_idx * record_size], &data[start_idx * record_size], record_size);
			continue;
		}

		//checkFullDocs(record_size, cmp_data, data_array, record_size, is_full, rdata);
		//aos_assert_r(crt_groupid < (sizeid + 1) * eMaxHeaderPerSizeId, false);

		rslt = saveGroupDocsFixed(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, rdata);
		aos_assert_r(rslt, false);

		crt_groupid = groupid;
		rslt = readGroupDocsFixed(docfilemgr, snap_id, crt_groupid, doc_buff, rdata);
		aos_assert_r(rslt, false);
		data_array = (char*)doc_buff->data();

		crt_array_idx = local_docid % max_docs_per_group;
		memcpy(&data_array[crt_array_idx * record_size], &data[start_idx * record_size], record_size);
	}

	//checkFullDocs(record_size, cmp_data, data_array, record_size, is_full, rdata);
	//aos_assert_r(crt_groupid < (sizeid +1)* eMaxHeaderPerSizeId, false);

	rslt = saveGroupDocsFixed(docfilemgr, trans_id, snap_id, crt_groupid, doc_buff, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosSengineGroupedDoc::saveGroupDocsFixed(
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const u64 &snap_id,
		const u32 &groupid, 
		const AosBuffPtr &doc_buff,
		const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, false);
	
	AosDfmDocPtr p_doc = readDocPriv(rdata, docfilemgr, snap_id, groupid, false);
	if (!p_doc) p_doc = OmnNew AosDfmGroupedDoc(siteid, groupid);

	bool is_compress = AosDocSvrObj::getDocSvr()->getGroupedDocWithComp();
	p_doc->setNeedCompress(is_compress);
	p_doc->setBodyBuff(doc_buff);

	bool rslt = saveDocPriv(rdata, docfilemgr, trans_id, snap_id, p_doc);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosSengineGroupedDoc::readGroupDocsFixed(
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const u32 &groupid,
		AosBuffPtr &doc_buff,
		const AosRundataPtr &rdata)
{
	//This function storage a group of docs that are the length is fixed. Each group has n docs.
	//Storage a group like a normal doc. it have body and header. the body contains all the doc.
	//and the header point to the body. read a group of doc have 6 step.
	//  1.calculate the header id by the docid. groupid=docid/num docs per group.
	//  2.calculate the header position by the groupid.
	//  3.read the body postion by the header
	//  4.read the body that is contains a group of doc.
	
	AosDfmDocPtr p_doc = readDocPriv(rdata, docfilemgr, snap_id, groupid, true); 
	if(!p_doc)
	{
		u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
		aos_assert_r(max_group_size > 0, false);
	
		doc_buff = OmnNew AosBuff(max_group_size AosMemoryCheckerArgs);
		doc_buff->setDataLen(max_group_size);
		memset(doc_buff->data(), 0, doc_buff->buffLen());
		return true;
	}

	doc_buff = p_doc->getBodyBuff();
	aos_assert_r(doc_buff && doc_buff->dataLen() > 0, false);

	return true;
}


bool
AosSengineGroupedDoc::readRecordFixed(
		const u64 &sizeid,
		const u64 &local_docid,
		char *data,
		const int record_size,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	// This function reads the doc 'local_id'. Fixed length docs are stored in groups. 
	// Each group is stored as one doc in 'docfilemgr'. 
	//Storage a group like a normal doc. it have body and header. the body contains all the doc.
	//and the header point to the body. read a doc have 6 step.
	//	1.calculate the header id by the docid. groupid=docid/num docs per group.
	//	2.calculate the header position by the groupid.
	//	3.read the body postion by the header
	//	4.read the body that is contains a group of doc.
	//	5.calculate the doc's offset by docid.
	//	6.read the doc in body by the offset
	u64 max_docs_per_group;
	bool rslt = getMaxDocsPerGroup(record_size, max_docs_per_group);
	aos_assert_r(rslt, false);

	u64 crt_groupid;
	rslt = getGroupid(sizeid, local_docid, max_docs_per_group, crt_groupid);
	aos_assert_r(rslt, false);
	
	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	AosBuffPtr doc_buff = OmnNew AosBuff(max_group_size AosMemoryCheckerArgs);
	rslt = readGroupDocsFixed(docfilemgr, 0, crt_groupid, doc_buff, rdata);
	aos_assert_r(rslt && doc_buff && doc_buff->dataLen() > 0, false);

	char *data_array = doc_buff->data();
	u32 offset = (local_docid % max_docs_per_group) * record_size;
	aos_assert_r(doc_buff->dataLen() >= offset + record_size, false);
	memcpy(data, &data_array[offset], record_size);

	return true;
}


bool
AosSengineGroupedDoc::readRecordsFixed(
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
	
	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	AosBuffPtr doc_buff = OmnNew AosBuff(max_group_size AosMemoryCheckerArgs);
	rslt = readGroupDocsFixed(docfilemgr, 0, crt_groupid, doc_buff, rdata);
	aos_assert_r(rslt && doc_buff && doc_buff->dataLen() > 0, false);

	u32 offset = (local_docid % max_docs_per_group) * record_size;
	int read_len = ((offset + max) > max_group_size) ? (max_group_size - offset) : max;
	aos_assert_r(read_len > 0 && (u32)read_len <= max, false);

	char *data_array = doc_buff->data();
	memcpy(buff->data(), &data_array[offset], read_len);

	if (read_len < 0) read_len = 0;
	buff->setDataLen(read_len);
	read_num_docs = read_len / record_size;
	return true;
}


AosXmlTagPtr 
AosSengineGroupedDoc::readFixedDocFromFile(
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

	// 2. Read from file
	char *mem = OmnMemMgrSelf->allocate(record_size + 1, __FILE__, __LINE__);
	aos_assert_r(mem, 0);

	bool rslt = readRecordFixed(sizeid, local_docid, mem, record_size, docfilemgr, rdata); 
	if (!rslt)
	{
		OmnAlarm << "failed reading file: " << docid << ":" << enderr;
		if (mem) OmnMemMgrSelf->release(mem, __FILE__, __LINE__);
		return 0;
	}

	OmnString docstr;
	docstr << "<doc "
		   << AOSTAG_DOCID << "=\"" << docid << "\" "
		   << AOSTAG_SITEID << "=\"" << rdata->getSiteid() << "\"><![CDATA[";
	docstr.append(mem, record_size);
	docstr << "]]></doc>";

	OmnMemMgrSelf->release(mem, __FILE__, __LINE__);

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	
	return doc;
}


AosBuffPtr
AosSengineGroupedDoc::readFixedDocsFromFile(
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
				rslt = batchReadFixedDocs(record_size, start_idx, num_docs, docids, crt_groupid, buff, rdata);
				aos_assert_r(rslt, 0);
			}
			crt_groupid = groupid;
			start_idx = i;
			num_docs = 0;
		}
	}
	
	if (num_docs > 0)
	{
		rslt = batchReadFixedDocs(record_size, start_idx, num_docs, docids, crt_groupid, buff, rdata);
		aos_assert_r(rslt, 0);
	}

	return buff;
}


bool
AosSengineGroupedDoc::batchReadFixedDocs(
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
	rslt = readGroupDocsFixed(doc_mgr, 0, crt_groupid, doc_buff, rdata);
	aos_assert_r(rslt && doc_buff && doc_buff->dataLen() > 0, false);

	char *data_array = doc_buff->data();
	u32 offset = 0;
	u64 local_docid = 0;
	for(u32 i=start_idx; i<start_idx + num_docs; i++)
	{
		local_docid = getLocalId(docids[i]);
		offset = (local_docid % max_docs_per_group) * record_size;
		aos_assert_r(doc_buff->dataLen() >= offset + record_size, false);

		buff->setU64(docids[i]);
		buff->setU32(record_size);
		buff->setBuff(&data_array[offset], record_size);
	}

	return true;
}


AosBuffPtr
AosSengineGroupedDoc::readFixedDocsFromFile(
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

	int record_size = record->getRecordLen();
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
	bool rslt = readRecordsFixed(sizeid, local_id,
		record_size, buff, max, num_docs, docfilemgr, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_read_file") << ": " << docid << enderr;
		buff->setDataLen(0);
		return buff;
	}
	return buff;
}


void
AosSengineGroupedDoc::checkFullDocs(
		const u32 len,
		const char *cmp_data,
		const char *data_array,
		const int &record_size,
		bool &is_full,
		const AosRundataPtr &rdata)
{
	// This function checks whether the current group is full (that is, 
	// all of its member docs are set). A member doc is set if its
	// memery is not all 0 (note that this may not be true in general!!!!!!!!!)
	u32 reserved_size = AosDocFileMgrObj::getReservedSize();
	u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
	if (reserved_size + len >= max_group_size) 
	{
		is_full = true;
		return;
	}

	u64 doc_per = (max_group_size - reserved_size) /record_size;
	u64 last_offset = (doc_per -1) * record_size + reserved_size; 

	aos_assert(last_offset + record_size <= max_group_size);
	aos_assert(last_offset + record_size + record_size > max_group_size);

	u64 check_offset = last_offset;
	while(1)
	{
		aos_assert(check_offset >= reserved_size && check_offset < max_group_size);
		if (memcmp(&data_array[check_offset], cmp_data, record_size) == 0)
		{
			is_full = false;
			return ;
		}

		if (check_offset <= reserved_size) break;
		check_offset -= record_size;
	}	

	aos_assert(check_offset == reserved_size);
	is_full = true;
}

