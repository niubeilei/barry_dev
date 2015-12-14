///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 01/18/2013	Created by  Ketty.
////////////////////////////////////////////////////////////////////////////

#include "DocFileMgr/DfmHeader.h"

#include "API/AosApi.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmDocHeader.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DocFileMgr/DfmHeaderFile.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/Mutex.h"

AosDfmHeader::AosDfmHeader(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &h_prefix,
		list<AosDfmHeaderFilePtr> &files,
		AosDfmDocType::E doc_type,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mVirtualId(v_id),
mDfmId(dfm_id),
//mSnapId(snap_id),
mPrefix(h_prefix),
mMaxSeqno(0),
mShowLog(show_log)
{
	//for(u32 i=0; i<files.size(); i++)
	list<AosDfmHeaderFilePtr>::iterator itr = files.begin();
	for(; itr != files.end(); itr++)
	{
		AosDfmHeaderFilePtr ff = *itr;
		//if(ff->getSnapId() != mSnapId)	continue;
		
		u32 seqno = ff->getFileSeqno();
		mFiles.insert(make_pair(seqno, ff));
		if(seqno > mMaxSeqno) mMaxSeqno = seqno;
	}

	mTempDoc = AosDfmDoc::cloneDoc(doc_type);
	mDocHeaderSize = mTempDoc->getHeaderSize();

	mCheckSumOff = eHeaderInfoOff + mDocHeaderSize; 
	mDocHeaderEntrySize = mCheckSumOff + sizeof(u8);
	mMaxHeaderPerFile = eMaxHeaderFileSize / mDocHeaderEntrySize;
}


AosDfmHeader::~AosDfmHeader()
{
}


/*
AosDfmDocPtr
AosDfmHeader::readHeader(
		const AosRundataPtr &rdata,
		const u64 docid,
		AosDfmHeaderStatus::E &sts)
{
	u32 seqno = docid / mMaxHeaderPerFile;
	u64 offset = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;
	
	AosBuffPtr header_buff = OmnNew AosBuff(mDocHeaderEntrySize AosMemoryCheckerArgs);
	u32 read_size = 0;
	bool rslt = readAndCheckHeader(rdata, seqno, offset, 
			mDocHeaderEntrySize, header_buff->data(), read_size);
	aos_assert_r(rslt, 0);
	header_buff->setDataLen(mDocHeaderEntrySize);
	
	sts = (AosDfmHeaderStatus::E)header_buff->getU32(0);
	AosDfmDocPtr doc = mTempDoc->clone(docid);
			
	if(sts != AosDfmHeaderStatus::eInvalidSts && sts != AosDfmHeaderStatus::eClean)
	{
		u32 header_len = header_buff->getU32(0);
		aos_assert_r(header_len && header_len <= mDocHeaderSize, 0);

		// We need to remove 'header_size' and 'sts'.
		char *data = header_buff->data();
		memmove(data, &data[eHeaderInfoOff], header_len);
		header_buff->setDataLen(header_len);
			
		doc->setHeaderBuff(header_buff);
	}
	aos_assert_r(sts<AosDfmHeaderStatus::eInvalidSts2, 0);
	return doc;
}
*/

/*
bool
AosDfmHeader::saveHeader(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		const AosDfmDocPtr &doc,
		const AosDfmHeaderStatus::E sts)
{
	u64 docid = doc->getDocid();
	AosBuffPtr header_buff = doc->getHeaderBuff();

	u32 seqno = docid / mMaxHeaderPerFile;
	u64 offset = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;
	
	u32 data_len = header_buff->dataLen();
	aos_assert_r(data_len <= mDocHeaderSize, false);
	
	AosReliableFilePtr ff = openHeaderFile(rdata, seqno, dfm);
	aos_assert_r(ff, false);

	u8 check_sum = AosCalculateCheckSum((char *)&sts, sizeof(u32)) +
			AosCalculateCheckSum((char *)&data_len, sizeof(u32)) +
			AosCalculateCheckSum(header_buff->data(), data_len);

	// Ketty 2013/07/09
	AosBuffPtr t_buff = OmnNew AosBuff(mDocHeaderEntrySize, 0 AosMemoryCheckerArgs);
	memset(t_buff->data(), 0, mDocHeaderEntrySize);
	t_buff->setU32(sts);
	t_buff->setU32(data_len);
	t_buff->setBuff(header_buff);
	t_buff->setCrtIdx(mCheckSumOff);
	t_buff->setU8(check_sum);
	ff->put(offset, t_buff->data(), mDocHeaderEntrySize, true, rdata);
	//ff->setU32(offset, sts, false, rdata);
	//ff->setU32(offset + eHeaderLenOff, data_len, false, rdata);
	//ff->put(offset + eHeaderInfoOff, header_buff->data(), data_len, false, rdata);
	//ff->put(offset + eHeaderInfoOff + data_len,
	//		mEmpHeaderBuff->data(), mDocHeaderSize - data_len, false, rdata);
	//ff->setU8(offset + mCheckSumOff, check_sum, true, rdata);

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmHeader; saveHeader:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; snap_id:" << mSnapId
			<< "; docid:" << docid 
			<< "; file:" << ff->getFileName()
			<< "; seqno:" << seqno
			<< "; offset: " << offset 
			<< "; check_sum:" << check_sum 
			<< "; header_len:" << data_len
			<< "; bodySeqno:" << doc->getBodySeqno() 
			<< "; bodyOffset:" << doc->getBodyOffset() 
			<< endl;
	}
	return true;
}


bool
AosDfmHeader::readHeaders(
		const AosRundataPtr &rdata,
		AosDfmCompareFunType::set_f &docs)
{
	AosDfmCompareFunType::setitr_f itr;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		AosDfmDocPtr doc = *itr;
		AosDfmHeaderStatus::E old_sts;
		AosDfmDocPtr old_doc = readHeader(rdata, doc->getDocid(), old_sts);
		if(old_sts == AosDfmHeaderStatus::eInvalidSts 
				|| old_sts == AosDfmHeaderStatus::eClean
				|| old_sts == AosDfmHeaderStatus::eDelete)
		{
			if (doc->getOpr() == AosDfmDoc::eSave)
			{
				doc->resetBodySeqOff(0, 0);
				doc->setOpr(AosDfmDoc::eNew);
				doc->setHeaderSts(AosDfmHeaderStatus::eNew);
			}
			else
			{
				// Ketty 2013/08/30
				//OmnAlarm << "this doc not exist in file."
				//	<< "docid:" << doc->getDocid() << enderr;
				doc->resetBodySeqOff(0, 0);
				doc->setHeaderSts(AosDfmHeaderStatus::eDelete);
			}
		}
		else
		{
			aos_assert_r(old_doc->getBodyOffset() != 0, false);
			doc->resetBodySeqOff(old_doc->getBodySeqno(), old_doc->getBodyOffset());
			if (doc->getOpr() == AosDfmDoc::eSave)
			{
				doc->setOpr(AosDfmDoc::eModify);
				doc->setHeaderSts(AosDfmHeaderStatus::eModify);
			}
			else
			{
				doc->setHeaderSts(AosDfmHeaderStatus::eDelete);
			}
		}
	}
	return true;
}


bool
AosDfmHeader::saveHeaders(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_f &docs)
{
	AosDfmCompareFunType::setitr_f itr;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		bool rslt = saveHeader(rdata, dfm, *itr, (*itr)->getHeaderSts());  
		if (!rslt) OmnAlarm << "save header "<< enderr;
	}
	return true;
}

*/


// Ketty 2014/03/03
/*
bool
AosDfmHeader::removeFiles(
		const AosRundataPtr &rdata, 
		const AosDocFileMgrObjPtr &dfm)
{
	mLock->lock();
	mFiles.clear();
	mLock->unlock();
		
	bool rslt = dfm->removeHeaderFiles(rdata, mSnapId);
	aos_assert_r(rslt, false);
	return true;	
}
*/


/*
bool
AosDfmHeader::saveBatchFixedDocs(
		const AosDocFileMgrObjPtr &dfm,
		const int num_docs,
		const u64 *docids, 
		const AosBuffPtr &buff, 
		const int record_size,
		const AosRundataPtr &rdata)
{
	// This function saves the docs into the file. There are 'num_docs'
	// number of docs. All are fixed length. Docs normally are arranged
	// adjacent to each other:
	// 		doc1
	// 		doc2
	// 		...
	//
	// 'docids': It is an array. The value 0 means there is no doc for that position.
	// 'buff': This is an array of fixed length docs. Each element holds a doc. 
	//         Not all elements are valid docs. If the corresponding docid in
	//         'docids' is 0, it is not a valid doc.
	//
	// This function assumes that all docs in 'docids' are in the same sizeid.
	// The docids in 'docids' are all local. 
	aos_assert_rr(num_docs > 0, rdata, false);
	aos_assert_rr(record_size > 0, rdata, false);

	u64 max_file_size = AosStorageEngineObj::getGroupedDocGroupSize();
	aos_assert_rr(max_file_size > 0, rdata, false);
	const char *data = buff->data();

	u32 first_seqno = 0;
	u64 first_offset = 0;
	int first_idx = 0;
	u32 len = 0;

	u32 seqno = 0;
	u64 offset = 0;
	int start_idx = 0;
	u64 local_docid = 0;
	
	AosReliableFilePtr ff;
	bool rslt;

	while (start_idx < num_docs)
	{
		local_docid = docids[start_idx];
		if (local_docid == 0)
		{
			if (len > 0)
			{
				ff->lock();
				ff->put(first_offset, &data[first_idx * record_size], len, false, rdata);
				ff->unlock();
				OmnScreen << "save batch fixed doc, seqno:" << first_seqno << ", offset:" << first_offset
					<< ", len:" << len << ",filename:" << ff->getFileName() << ", docfilemgr_id:" << mDfmId << endl;
			}
			len = 0;
			start_idx++;
			continue;
		}

		rslt = getPosition(local_docid, record_size,
				seqno, offset, max_file_size);
		aos_assert_r(rslt, false);
		
		if (len == 0)
		{
			ff = openHeaderFile(rdata, seqno, dfm);
			aos_assert_r(ff, false);
			
			first_seqno = seqno;
			first_offset = offset;
			first_idx = start_idx;
			len +=record_size;
			start_idx++;
			continue;
		}

		if(first_seqno != seqno)
		{
			if(len > 0)
			{
				ff->lock();
				ff->put(first_offset, &data[first_idx * record_size], len, true, rdata);
				ff->unlock();
				OmnScreen << "save batch fixed doc, seqno:" << first_seqno << ", offset:" << first_offset
					<< ", len:" << len << ",filename:" << ff->getFileName() << ", docfilemgr_id:" << mDfmId << endl;
			}
		
			ff = openHeaderFile(rdata, seqno, dfm);
			aos_assert_r(ff, false);
			
			first_seqno = seqno;
			first_offset = offset;
			first_idx = start_idx;
			len = record_size;
			start_idx++;
			continue;
		}

		if(local_docid != docids[start_idx - 1] + 1)
		{
			OmnAlarm << "docid error" << enderr;
			return false;
		}
		
		len += record_size;
		start_idx++;
	}	

	if(len > 0)
	{
		ff->lock();
		ff->put(first_offset, &data[first_idx * record_size], len, true, rdata);
		ff->unlock();
		OmnScreen << "save batch fixed doc, seqno:" << first_seqno << ", offset:" << first_offset
			<< ", len:" << len << ",filename:" << ff->getFileName() << ", docfilemgr_id:" << mDfmId << endl;
	}
	return true;
}
*/
/*
bool
AosDfmHeader::readHeader(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		const bool force)
{
	doc->cleanBodySeqOff();

	u64 docid = doc->getDocid();
	u32 seqno = docid / mMaxHeaderPerFile;
	u64 offset = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;
	
	AosBuffPtr header_buff = OmnNew AosBuff(mDocHeaderEntrySize AosMemoryCheckerArgs);
	u32 read_size = 0;
	bool rslt = readAndCheckHeader(rdata, seqno, offset, 
			mDocHeaderEntrySize, header_buff->data(), read_size);
	aos_assert_r(rslt, false);

	if(memcmp(header_buff->data(), mEmpHeaderBuff->data(), mDocHeaderEntrySize) == 0)
	{
		// means the header_buff is empty.
		return true;
	}

	header_buff->setDataLen(mDocHeaderEntrySize);
	Status sts = (Status)header_buff->getU32(0);
	
	if(sts == eClean)	return true;
	if(sts == eDelete && !force) return true;

	u32 header_len = header_buff->getU32(0);
	aos_assert_r(header_len, false);

	// 'header_buff' is in the following format:
	//  sts_flag (u32)
	// 	header_size (u32)
	// 	header_contents (variable)
	// 	header_checksum (xxx)
	// We need to remove 'header_size'.
	char *data = header_buff->data();
	memmove(data, &data[eHeaderInfoOff], header_len);
	header_buff->setDataLen(header_len);
		
	doc->setHeaderBuff(header_buff);
	return true;
}
*/


/*
 * Linda 2013/07/30

bool
AosDfmHeader::readHeaders(
		const AosRundataPtr &rdata,
		const u64 beg_docid,
		const u32 total_num,
		//AosBuffPtr &headers_buff)
		vector<AosDfmDocPtr> &all_doc,
		vector<Status> &all_sts)
{
	//u32 buff_len = total_num * (mDocHeaderEntrySize + sizeof(u64)) + 10;
	//headers_buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	//memset(headers_buff->data(), 0, buff_len);
	all_doc.clear();
	all_sts.clear();

	u32 expect_num = eAosSizePerRead / mDocHeaderEntrySize;
	AosBuffPtr data_buff = OmnNew AosBuff(expect_num * mDocHeaderEntrySize, 0 AosMemoryCheckerArgs);
	
	u64 docid = beg_docid;
	u32 need_num = total_num;
	while(need_num)
	{
		u32 seqno = docid/ mMaxHeaderPerFile;
		u64 f_off = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;

		u32 read_num =  mMaxHeaderPerFile - docid % mMaxHeaderPerFile;
		if(read_num > need_num) read_num = need_num;
		if(read_num > expect_num) read_num = expect_num;

		u32 act_r_size = 0;
		data_buff->setCrtIdx(0);
		aos_assert_r(read_num * mDocHeaderEntrySize <= data_buff->buffLen(), false);
		bool rslt = readAndCheckHeader(rdata, seqno, f_off,
				read_num * mDocHeaderEntrySize, data_buff->data(), act_r_size);
		aos_assert_r(rslt && act_r_size % mDocHeaderEntrySize == 0, false);
		data_buff->setDataLen(act_r_size);
		u32 act_r_num = act_r_size / mDocHeaderEntrySize;

		for(u32 i=0; i<act_r_num; i++)
		{
			AosBuffPtr hb = data_buff->getBuff(mDocHeaderEntrySize, false AosMemoryCheckerArgs);
			
			if(memcmp(hb->data(), mEmpHeaderBuff->data(), mDocHeaderEntrySize) == 0)
			{
				continue; // means the header_buff is empty.
			}
			
			Status sts = (Status)hb->getU32(0);
			if(sts == eClean)	continue;

			u64 crt_did = docid + i;
			u32 header_len = hb->getU32(0);
			aos_assert_r(sts != eInvalidSts && header_len 
					&& header_len <= mDocHeaderSize, false);

			//char * cont = hb->data() + hb->getCrtIdx();	
			//headers_buff->setU64(crt_did);
			//headers_buff->setU32(sts);
			//headers_buff->setU32(header_len);
			//headers_buff->setBuff(cont, header_len);

			AosDfmDocPtr dd = mTempDoc->clone(crt_did);
			//dd->setHeaderBuff(hb->getBuff(header_len, true AosMemoryCheckerArgs));
			AosBuffPtr h_data = hb->getBuff(mDocHeaderSize, true AosMemoryCheckerArgs);
			h_data->setDataLen(header_len);
			dd->setHeaderBuff(h_data);
			
			all_doc.push_back(dd);
			all_sts.push_back(sts);

			need_num--;
		
			if(mShowLog)
			{
				OmnScreen << "DocFileMgr; DfmHeader; readHeaders:"
					<< "; virtual_id:" << mVirtualId
					<< "; dfm_id:" << mDfmId
					<< "; docid:" << crt_did 
					<< "; snap_id:" << mSnapId
					<< "; sts:" << sts
					<< "; header_len:" << header_len
					<< endl;
			}
		}

		if(act_r_num < need_num && seqno >= mMaxSeqno)
		{
			break;	 // means read finish.
		}

		docid += read_num;
	}

	//headers_buff->setCrtIdx(0);
	return true;
}


bool
AosDfmHeader::saveHeaderToBuff(
		const AosBuffPtr &headers_buff,
		const u32 idx,
		const AosDfmDocPtr &doc,
		const Status sts)
{
	AosBuffPtr h_buff = doc->getHeaderBuff();
	u32 data_len = h_buff->dataLen();
	aos_assert_r(data_len <= mDocHeaderSize, false);
	
	u32 offset = idx * mDocHeaderEntrySize;
	aos_assert_r(offset < (u32)headers_buff->buffLen(), false);
	
	u8 check_sum = AosCalculateCheckSum((char *)&sts, sizeof(u32)) +
			AosCalculateCheckSum((char *)&data_len, sizeof(u32)) +
			AosCalculateCheckSum(h_buff->data(), data_len);

	headers_buff->setCrtIdx(offset);
	headers_buff->setU32(sts);
	headers_buff->setU32(data_len);
	headers_buff->setBuff(h_buff);
	headers_buff->setBuff(mEmpHeaderBuff->data(), mDocHeaderSize - data_len);
	headers_buff->setU8(check_sum);
	
	if(mShowLog)
	{
		OmnScreen << "docfilemgr; dfmheader; saveToBuff:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; snap_id:" << mSnapId
			<< "; buff_idx:" << idx 
			<< "; offset: " << offset
			<< "; sts:" << sts
			<< "; data_len:" << data_len 
			<< "; check_sum:" << check_sum 
			<< endl;
	}
	return true;
}


AosDfmDocPtr
AosDfmHeader::readHeaderFromBuff(
		const AosBuffPtr &headers_buff,
		const u32 idx,
		const u64 docid,
		Status &sts,
		const bool clone)
{
	u32 offset = idx * mDocHeaderEntrySize;
	aos_assert_r(offset < (u32)headers_buff->buffLen(), 0);
	headers_buff->setCrtIdx(offset);
	
	AosDfmDocPtr doc = mTempDoc->clone(docid);
			
	sts = (Status)headers_buff->getU32(0);
	if(sts != AosDfmHeader::eInvalidSts && sts != AosDfmHeader::eClean)
	{
		u32 header_len = headers_buff->getU32(0);
		aos_assert_r(header_len && header_len <= mDocHeaderSize, 0);
		
		//AosBuffPtr buff = headers_buff->getBuff(header_len, clone AosMemoryCheckerArgs);
		AosBuffPtr buff = headers_buff->getBuff(mDocHeaderSize, clone AosMemoryCheckerArgs);
		buff->setDataLen(header_len);
		doc->setHeaderBuff(buff);
	}
	return doc;
}

bool
AosDfmHeader::readHeaders(
		const AosRundataPtr &rdata,
		AosDfmCompareFunType::set_f &docs)
{
	// Linda, 2013/07/31
	AosDfmCompareFunType::setitr_f itr;
	u32 total_num = eMaxReadHeaderSize / mDocHeaderEntrySize;
	AosBuffPtr header_buff = OmnNew AosBuff(eMaxReadHeaderSize AosMemoryCheckerArgs);
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		u64 begin_docid = (*itr)->getDocid();
		u64 end_docid = begin_docid + total_num -1; 

		readHeaders(rdata, begin_docid, header_buff);
		for (;itr != docs.end(); ++itr) 
		{
			u64 docid = (*itr)->getDocid();
			aos_assert_r(begin_docid <= docid, false);

			if (docid > end_docid) break;

			u32 idx = docid - begin_docid;
			readHeaderFromBuff(header_buff, *itr, idx, true);
		}
		if (itr == docs.end()) break;
	}
	return true;
}


bool
AosDfmHeader::readHeaderFromBuff(
		const AosBuffPtr &headers_buff,
		const AosDfmDocPtr &doc,
		const u32 idx,
		const bool clone)
{
	// Linda, 2013/07/31
	u32 offset = idx * mDocHeaderEntrySize;
	aos_assert_r(offset < (u32)headers_buff->buffLen(), 0);
	headers_buff->setCrtIdx(offset);
	
	AosDfmHeaderStatus::E sts = (AosDfmHeaderStatus::E)headers_buff->getU32(0);
	if(sts != AosDfmHeaderStatus::eInvalidSts && sts != AosDfmHeaderStatus::eClean)
	{
		u32 header_len = headers_buff->getU32(0);
		aos_assert_r(header_len && header_len <= mDocHeaderSize, 0);
		
		AosBuffPtr buff = headers_buff->getBuff(mDocHeaderSize, clone AosMemoryCheckerArgs);
		buff->setDataLen(header_len);
		doc->setHeaderBuff(buff);

		// eSave convert eModify
		if (doc->getOpr() == AosDfmDoc::eSave) 
		{
			doc->setOpr(AosDfmDoc::eModify);
			doc->setHeaderSts(AosDfmHeaderStatus::eModify);
		}
		else
		{
			doc->setHeaderSts(AosDfmHeaderStatus::eDelete);
		}
	}
	else
	{
		// eSave convert eNew
		doc->resetBodySeqOff(0, 0);
		doc->setOpr(AosDfmDoc::eNew);
		doc->setHeaderSts(AosDfmHeaderStatus::eNew);
	}
	return true;
}


bool
AosDfmHeader::readHeaders(
		const AosRundataPtr &rdata,
		const u64 beg_docid,
		const AosBuffPtr &headers_buff)
{
	// Linda, 2013/07/31
	aos_assert_r(headers_buff, false);
	u32 buff_len = headers_buff->buffLen();
	memset(headers_buff->data(), 0, buff_len);
	u32 total_num = buff_len / mDocHeaderEntrySize;

	u32 expect_num = eAosSizePerRead / mDocHeaderEntrySize;
	u32 docid = beg_docid;

	u32 buff_idx = 0;
	u32 need_num = total_num;
	while(need_num)
	{
		u32 seqno = docid/ mMaxHeaderPerFile;
		u64 f_off = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;

		u32 read_num =  mMaxHeaderPerFile - docid % mMaxHeaderPerFile; 
		if(read_num > need_num)	read_num = need_num;
		if(read_num > expect_num) read_num = expect_num;	

		u32 act_r_size = 0;
		u32 data_off = buff_idx * mDocHeaderEntrySize;       
		u32 read_size = read_num * mDocHeaderEntrySize; 
		aos_assert_r(read_size + data_off <= buff_len, false);
		char * data = headers_buff->data() + data_off;
		bool rslt = readAndCheckHeader(rdata, seqno, f_off,
				        read_size, data, act_r_size);

		aos_assert_r(rslt && act_r_size % mDocHeaderEntrySize == 0, false);
	
		u32 act_r_num = act_r_size / mDocHeaderEntrySize;
		if(act_r_num < read_num && seqno == mMaxSeqno)
		{
			// means read finish.
			need_num -= act_r_num;
			break;
		}
	
		//if act_r_num != read_num means this file remain has no data.
		buff_idx += read_num;
		need_num -= read_num;
		docid += read_num;
	}

	headers_buff->setDataLen(total_num * mDocHeaderEntrySize);
	headers_buff->setCrtIdx(0);
	return true;
}


bool
AosDfmHeader::saveHeaders(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_f &docs)
{
	// Linda, 2013/07/31
	AosDfmCompareFunType::setitr_f itr;
	u32 total_num = eMaxReadHeaderSize / mDocHeaderEntrySize;
	AosBuffPtr header_buff = OmnNew AosBuff(eMaxReadHeaderSize AosMemoryCheckerArgs);

	u64 docid = 0;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		u64 begin_docid = (*itr)->getDocid();
		u64 end_docid = begin_docid + total_num -1; 
		readHeaders(rdata, begin_docid, header_buff);
		for (;itr != docs.end(); ++itr) 
		{
			docid = (*itr)->getDocid();
			aos_assert_r(begin_docid <= docid, false);

			if (docid > end_docid) break;

			u32 idx = docid - begin_docid;
			saveHeaderToBuff(header_buff, idx, (*itr), (*itr)->getHeaderSts());
		}
		saveHeaders(rdata, dfm, begin_docid, header_buff);
		if (itr == docs.end()) break;
	}
	return true;
}

bool
AosDfmHeader::saveHeaderToBuff(
		const AosBuffPtr &headers_buff,
		const u32 idx,
		const AosDfmDocPtr &doc,
		const AosDfmHeaderStatus::E sts)
{
	// Linda, 2013/07/31
	AosBuffPtr h_buff = doc->getHeaderBuff();
	u32 data_len = h_buff->dataLen();
	aos_assert_r(data_len <= mDocHeaderSize, false);
	
	u32 offset = idx * mDocHeaderEntrySize;
	aos_assert_r(offset < (u32)headers_buff->buffLen(), false);
	
	u8 check_sum = AosCalculateCheckSum((char *)&sts, sizeof(u32)) +
			AosCalculateCheckSum((char *)&data_len, sizeof(u32)) +
			AosCalculateCheckSum(h_buff->data(), data_len);

	headers_buff->setCrtIdx(offset);
	headers_buff->setU32(sts);
	headers_buff->setU32(data_len);
	headers_buff->setBuff(h_buff);
	headers_buff->setBuff(mEmpHeaderBuff->data(), mDocHeaderSize - data_len);
	headers_buff->setU8(check_sum);
	
	if(mShowLog)
	{
		OmnScreen << "docfilemgr; dfmheader; saveToBuff:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; snap_id:" << mSnapId
			<< "; buff_idx:" << idx 
			<< "; offset: " << offset
			<< "; sts:" << sts
			<< "; data_len:" << data_len 
			<< "; check_sum:" << check_sum 
			<< endl;
	}
	return true;
}


bool
AosDfmHeader::saveHeaders(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		const u64 beg_docid,
		const AosBuffPtr &headers_buff)
{
	// Linda, 2013/07/31
	aos_assert_r(headers_buff, false);
	u32 data_len = headers_buff->dataLen();
	aos_assert_r(data_len % mDocHeaderEntrySize == 0, false);
	u32 total_num = data_len / mDocHeaderEntrySize;

	u32 expect_num  = eAosSizePerRead / mDocHeaderEntrySize;
	u32 docid = beg_docid;
	
	u32 buff_idx = 0;
	u32 remain_num = total_num;
	while(remain_num)
	{
		u32 seqno = docid/ mMaxHeaderPerFile;
		u64 f_off= (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;
		
		u32 save_num = mMaxHeaderPerFile - docid % mMaxHeaderPerFile;
		if(save_num > remain_num)	save_num = remain_num;
		if(save_num > expect_num)	save_num = expect_num;
		
		AosReliableFilePtr ff = openHeaderFile(rdata, seqno, dfm);
		aos_assert_r(ff, false);
		
		char *data = headers_buff->data() + buff_idx * mDocHeaderEntrySize;
		ff->put(f_off, data, save_num * mDocHeaderEntrySize, true, rdata);
		if(mShowLog)
		{
			OmnScreen << "DocFileMgr; DfmHeader; saveHeaders:"
				<< "; virtual_id:" << mVirtualId
				<< "; dfm_id:" << mDfmId
				<< "; snap_id:" << mSnapId
				<< "; file:" << ff->getFileName()
				<< "; offset: " << f_off
				<< ": sizes:" << save_num * mDocHeaderEntrySize 
				<< endl;
		}
		
		buff_idx += save_num;
		remain_num -= save_num;
		docid += save_num;
	}
	
	return true;
}


*/

// Ketty 2014/03/03
bool
AosDfmHeader::readHeader(
		const AosRundataPtr &rdata,
		const u64 docid,
		AosDfmDocHeaderPtr &header)
{
	u32 seqno = docid / mMaxHeaderPerFile;
	u64 offset = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;
	
	AosBuffPtr header_buff = OmnNew AosBuff(mDocHeaderEntrySize AosMemoryCheckerArgs);
	u32 read_size = 0;
	bool rslt = readAndCheckHeader(rdata, seqno, offset, 
			mDocHeaderEntrySize, header_buff->data(), read_size);
	aos_assert_r(rslt, 0);
	header_buff->setDataLen(mDocHeaderEntrySize);
		
	EntrySts sts = (EntrySts)header_buff->getU32(0);
	if(sts == eInvalidEntry)
	{
		// means not exist.
		header = mTempDoc->cloneDfmHeader(docid);
		return true;
	}

	u32 header_len = header_buff->getU32(0);
	aos_assert_r(header_len && header_len <= mDocHeaderSize, 0);

	char *data = header_buff->data();
	memmove(data, &data[eHeaderInfoOff], header_len);
	header_buff->setDataLen(header_len);

	header = mTempDoc->cloneDfmHeader(docid, header_buff);
	return true;
}

bool
AosDfmHeader::readAndCheckHeader(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u64 offset,
		const u32 need_size,
		char * data,
		u32 &read_size)
{
	memset(data, 0, need_size);
	
	read_size = 0;
	AosReliableFilePtr rfile = openHeaderFile(rdata, seqno);
	if(!rfile) return true;
	
	u32 len = rfile->getLength();
	if(offset >= len)	return true;
	
	u32 remain_size = len - offset;
	read_size = remain_size < need_size ? remain_size : need_size;
	bool rslt = rfile->readToBuff(offset, read_size, data, rdata.getPtr());
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmHeader; readHeader:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; seqno: " << seqno
			<< "; offset: " << offset 
			<< "; need_size: " << need_size 
			<< "; read_size: " << read_size 
			<< "; file:" << rfile->getFileName()
			<< "; file_len:" << rfile->getLength()
			//<< "; bodySeqno:" << seqno
			//<< "; bodyOffset:" << offset
			<< endl;
	}

	bool data_good = AosCheckCheckSum(data, read_size);
	if(!data_good)
	{
		//OmnString data_u8;
		////for(u32 i=0; i<len; i++)
		//for(u32 i=0; i<read_size; i++)
		//{
		//	if(data[i] != 0) data_u8 << data[i] << "; ";	
		//}
		OmnScreen << "DocFileMgr; DfmHeader; readHeader:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; seqno: " << seqno
			<< "; offset: " << offset 
			<< "; need_size: " << need_size 
			<< "; read_size: " << read_size 
			<< "; file:" << rfile->getFileName()
			<< "; file_len: " << rfile->getLength()
		//	<< "; data: " << data_u8
			<< endl;
	
		// Ketty temp.
		u32 num = read_size / mDocHeaderEntrySize;
		for(u32 i=0; i<num; i++)
		{
			char * c_data = data + i * mDocHeaderEntrySize;
			data_good = AosCheckCheckSum(c_data, mDocHeaderEntrySize);
			aos_assert_r(data_good, false);
		}

		rslt = rfile->recover(rdata.getPtr());
		aos_assert_r(rslt, false);

		rslt = rfile->readToBuff(offset, read_size, data, rdata.getPtr());
		aos_assert_r(rslt, false);
		data_good = AosCheckCheckSum(data, read_size);
		if(!data_good)
		{
			AosSetErrorU(rdata, "file_not_good") << ": " << rfile->getFileId() << enderr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	return true;
}

	
bool
AosDfmHeader::saveHeader(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		const AosDfmDocHeaderPtr &header)
{
	u64 docid = header->getDocid();
	AosBuffPtr header_buff = header->getHeaderBuff();
	aos_assert_r(header_buff, false);

	u32 seqno = docid / mMaxHeaderPerFile;
	u64 offset = (docid % mMaxHeaderPerFile) * mDocHeaderEntrySize;
	
	u32 data_len = header_buff->dataLen();
	aos_assert_r(data_len <= mDocHeaderSize, false);
	
	AosReliableFilePtr ff = openHeaderFile(rdata, seqno, dfm);
	aos_assert_r(ff, false);

	// Ketty 2013/07/09
	AosBuffPtr t_buff = OmnNew AosBuff(mDocHeaderEntrySize, 0 AosMemoryCheckerArgs);
	memset(t_buff->data(), 0, mDocHeaderEntrySize);
	
	EntrySts sts = eValidEntry;
	if(header->getBodySeqno() == 0 && header->getBodyOffset() == 0)
	{
		// means this doc has deleted.
		sts = eInvalidEntry;
	}
	
	t_buff->setU32(sts);
	t_buff->setU32(data_len);
	t_buff->setBuff(header_buff);
	
	t_buff->setCrtIdx(mCheckSumOff);

	u8 check_sum = AosCalculateCheckSum((char *)&sts, sizeof(u32)) +
			AosCalculateCheckSum((char *)&data_len, sizeof(u32)) +
			AosCalculateCheckSum(header_buff->data(), data_len);
	//u8 check_sum = AosCalculateCheckSum((char *)&data_len, sizeof(u32)) +
	//		AosCalculateCheckSum(header_buff->data(), data_len);
	t_buff->setU8(check_sum);
	ff->put(offset, t_buff->data(), mDocHeaderEntrySize, true, rdata.getPtr());

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmHeader; saveHeader:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << docid 
			<< "; file:" << ff->getFileName()
			<< "; seqno:" << seqno
			<< "; offset: " << offset 
			<< "; check_sum:" << check_sum 
			<< "; header_len:" << data_len
			<< "; bodySeqno:" << header->getBodySeqno() 
			<< "; bodyOffset:" << header->getBodyOffset() 
			<< endl;
	}
	return true;
}


bool
AosDfmHeader::readHeaders(
		const AosRundataPtr &rdata,
		AosDfmCompareFunType::set_f &docs)
{
	// modifyed by Ketty 2014/03/03
	AosDfmDocHeaderPtr crt_header;
	bool rslt;
	AosDfmCompareFunType::setitr_f itr;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		AosDfmDocPtr doc = *itr;
		rslt = readHeader(rdata, doc->getDocid(), crt_header);
		aos_assert_r(rslt, false);	
		
		doc->setBodySeqOff(crt_header->getBodySeqno(),
			crt_header->getBodyOffset());
		
		if(crt_header->isEmpty())
		{
			if(doc->getOpr() == AosDfmDoc::eSave)
			{
				doc->setOpr(AosDfmDoc::eNew);	
			}
					
			// Ketty 2013/08/30
			//OmnAlarm << "this doc not exist in file."
			//	<< "docid:" << doc->getDocid() << enderr;
			continue;
		}

		if (doc->getOpr() == AosDfmDoc::eSave)
		{
			doc->setOpr(AosDfmDoc::eModify);
		}
	}
	return true;
}


bool
AosDfmHeader::saveHeaders(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_f &docs)
{
	// modifyed by Ketty 2014/03/03
	
	AosDfmCompareFunType::setitr_f itr;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		//bool rslt = saveHeader(rdata, dfm, (*itr)); 
		bool rslt = saveHeader(rdata, dfm, (*itr)->getHeader()); 
		if (!rslt) OmnAlarm << "save header "<< enderr;
	}
	return true;
}


AosReliableFilePtr
AosDfmHeader::openHeaderFile(
		const AosRundataPtr &rdata, 
		const u32 seqno) 
{
	mLock->lock();
	AosReliableFilePtr ff;
	FileMapItr itr = mFiles.find(seqno);
	if(itr != mFiles.end())
	{
		AosDfmHeaderFilePtr h_file = itr->second;
		ff = h_file->getFile(rdata);
	}
	mLock->unlock();

	return ff;
}


AosReliableFilePtr
AosDfmHeader::openHeaderFile(
		const AosRundataPtr &rdata, 
		const u32 seqno,
		const AosDocFileMgrObjPtr &dfm)
{
	AosReliableFilePtr file = openHeaderFile(rdata, seqno);
	if(file)	return file;

	// create a file.
	mLock->lock();
	if(seqno > mMaxSeqno) mMaxSeqno = seqno;

	//AosDfmHeaderFilePtr dfm_file = OmnNew AosDfmHeaderFile(rdata, seqno, 
	//		mSnapId, mVirtualId, mPrefix, eMaxHeaderFileSize);
	AosDfmHeaderFilePtr dfm_file = OmnNew AosDfmHeaderFile(rdata, seqno, 
			0, mVirtualId, mPrefix, eMaxHeaderFileSize);
	aos_assert_rl(dfm_file, mLock, 0);

	mFiles.insert(make_pair(seqno, dfm_file));
	mLock->unlock();
	
	bool rslt = dfm->addHeaderFile(rdata, dfm_file);
	aos_assert_r(rslt, 0);
	file = dfm_file->getFile(rdata);
	return file;
}


