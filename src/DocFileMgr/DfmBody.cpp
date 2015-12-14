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

#include "DocFileMgr/DfmBody.h"

#include "API/AosApi.h"
#include "CompressUtil/Compress.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmDocHeader.h"
#include "DocFileMgr/DiskBlockMgr.h"
#include "DocFileMgr/SlabBlockMgr.h"
#include "DocFileMgr/ReadOnlyBlock.h"
#include "SEInterfaces/RlbFileType.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "StorageMgr/SystemId.h"
#include "Thread/Mutex.h"

static bool smShowLogBody = false;
OmnString AosDfmBody::smCompressAlg = "gzip";

AosDfmBody::AosDfmBody(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		const OmnString &compress_alg,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mVirtualId(v_id),
mDfmId(dfm_id),
mShowLog(show_log)
{
	smCompressAlg = compress_alg;
}


AosDfmBody::AosDfmBody(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		const OmnString &compress_alg,
		vector<AosDfmFilePtr> &files,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mVirtualId(v_id),
mDfmId(dfm_id),
mShowLog(show_log)
{
	smCompressAlg = compress_alg;

	OmnString disk_prefix,  slab_prefix = prefix, rd_prefix = prefix;
	slab_prefix << "_slab";
	rd_prefix << "_ro";

	//mDiskBlockMgr = OmnNew AosDiskBlockMgr(v_id, dfm_id, disk_prefix, files, mShowLog);
	mSlabBlockMgr = OmnNew AosSlabBlockMgr(v_id, dfm_id, slab_prefix, files, mShowLog);
	mReadOnlyBlock = OmnNew AosReadOnlyBlock(v_id, dfm_id, rd_prefix, files, mShowLog);
}


AosDfmBody::~AosDfmBody()
{
}


bool
AosDfmBody::stop(const AosRundataPtr &rdata)
{
	return false;
	//return mDiskBlockMgr->stop(rdata);
}

bool
AosDfmBody::readBody(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	u32 seqno = doc->getBodySeqno();
	u64 offset = doc->getBodyOffset();
	if(seqno == 0 && offset == 0) return true;
	
	bool rslt = false;
	AosBuffPtr data_buff;

	u32 orig_len = 0;
	u32 compress_len = 0;

	rslt = mSlabBlockMgr->readDoc(seqno, offset, data_buff, orig_len, compress_len, rdata);
	aos_assert_rr(rslt, rdata, false);

	rslt = uncompressBody(data_buff, orig_len, compress_len, rdata AosMemoryCheckerArgs);
	aos_assert_rr(rslt && data_buff, rdata, false);

	doc->setBodyBuff(data_buff);
	doc->setOrigLen(orig_len);
	doc->setCompressLen(orig_len);
	return true;
}


bool
AosDfmBody::addBody(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		const AosDfmDocPtr &doc)
{
	AosBuffPtr body_buff = doc->getBodyBuff();

	/*
	u32 compress_len = 0;
	u32 orig_len = body_buff->dataLen();
	if(doc->needCompress())
	{
		bool rslt = compressBody(body_buff, rdata AosMemoryCheckerArgs);
		aos_assert_r(rslt && body_buff, false);
		compress_len = body_buff->dataLen(); 
		doc->setBodyBuff(body_buff);	// reset the compressed body.
	}
	doc->setOrigLen(orig_len);
	doc->setCompressLen(compress_len);
	*/	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; add:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << doc->getDocid()
			<< "; orig_len:" << doc->getOrigLen()
			<< "; compress_len:" << doc->getCompressLen()
			<< endl;
	}

	bool rslt = findBlock(rdata, dfm, doc);
	aos_assert_r(rslt, false);
	rslt = mSlabBlockMgr->addDoc(doc, rdata);
	return true;
}

bool
AosDfmBody::findBlock(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		const AosDfmDocPtr &doc)
{
	aos_assert_r(doc->getBodySeqno() == 0 && doc->getBodyOffset() == 0, false);

	//Linda, 2013/08/27
	u32 orig_len = doc->getOrigLen();
	u32 compress_len = doc->getCompressLen();

AosBuffPtr body_buff = doc->getBodyBuff();
aos_assert_r(body_buff, false);

int64_t data_len1 = compress_len ? compress_len : orig_len;
aos_assert_r(body_buff->dataLen() == data_len1, false);  

	u32 seqno = 0;
	u32 blockId = 0;                                             
	u32 data_len = compress_len ? compress_len : orig_len;
	bool rslt = mSlabBlockMgr->findBlock(dfm, seqno, blockId, data_len, rdata); 
	aos_assert_r(rslt, false);
	doc->setBodySeqOff(seqno, blockId);
	return true;
}



bool
AosDfmBody::modifyBody(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		bool &kickout,
		const AosDfmDocPtr &doc)
{
	u32 seqno = doc->getBodySeqno();
	u64 offset = doc->getBodyOffset();
	aos_assert_r(seqno !=0 || offset != 0, false);

	//Linda, 2013/08/27
	u32 orig_len = doc->getOrigLen();
	u32 compress_len = doc->getCompressLen();

	AosBuffPtr body_buff = doc->getBodyBuff();
	aos_assert_r(body_buff, false);
int64_t data_len = compress_len ? compress_len : orig_len;
aos_assert_r(body_buff->dataLen() == data_len, false);  
	
	kickout = false;
	bool rslt = mSlabBlockMgr->modifyDoc(seqno, offset, orig_len, 
			compress_len, body_buff->data(), kickout, rdata);
	aos_assert_r(rslt, false);
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; modify:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << doc->getDocid()
			<< "; bodySeqno:" << seqno
			<< "; bodyOffset:" << offset
			<< "; orig_len:" << orig_len
			<< "; compress_len:" << compress_len
			<< "; kickout:" << (kickout?"true":"false")
			<< endl;
	}

	return true;
}


bool
AosDfmBody::removeBody(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	AosDfmDocHeaderPtr crt_header = doc->getHeader();
	bool rslt = removeBody(rdata, crt_header);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDfmBody::removeBody(
		const AosRundataPtr &rdata,
		const AosDfmDocHeaderPtr &header)
{
	aos_assert_r(header, false);
	u32 seqno = header->getBodySeqno();
	u64 offset = header->getBodyOffset();
	if (seqno == 0 && offset == 0)
	{
		// Ketty 2013/08/30
		//OmnAlarm << "this doc not exist in file."
		//	  << "docid:" << doc->getDocid() << enderr;
		return true;
	}
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; remove:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << header->getDocid()
			<< "; bodySeqno:" << seqno
			<< "; bodyOffset:" << offset
			<< endl;
	}

	bool rslt = mSlabBlockMgr->removeDoc(seqno, offset, rdata);	
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDfmBody::procBody(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_fs &docs)
{
	AosDfmCompareFunType::set_fs new_docs;
	AosDfmCompareFunType::set_fs delete_docs;

	// 1.modify 
	bool rslt = modifyBodys(rdata, dfm, docs, new_docs, delete_docs);
	aos_assert_r(rslt, false);

	// 2.delete
	rslt = removeBodys(rdata, delete_docs);
	aos_assert_r(rslt, false);

	// 3. create
	rslt = addBodys(rdata, dfm, new_docs);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDfmBody::modifyBodys(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_fs &docs,
		AosDfmCompareFunType::set_fs &new_docs,
		AosDfmCompareFunType::set_fs &delete_docs)
{
	AosDfmCompareFunType::setitr_fs add_sitr, del_sitr, itr;
	add_sitr = del_sitr = docs.end();

	int mod_num = 0;
	for (itr = docs.begin(); itr != docs.end(); ++itr)
	{
		if((*itr)->getOpr() != AosDfmDoc::eModify) 
		{
			if ((*itr)->getOpr() == AosDfmDoc::eDelete && del_sitr == docs.end()) del_sitr = itr; 
			if ((*itr)->getOpr() == AosDfmDoc::eNew && add_sitr == docs.end())  add_sitr = itr;
			continue;
		}

		bool kickout = false;
		bool rslt = modifyBody(rdata, dfm, kickout, (*itr));
		aos_assert_r(rslt, false);
		if (kickout)
		{
			// Ketty 2014/03/04
			//AosDfmDocType::E type = (*itr)->getType();
			//AosDfmDocPtr old_doc = AosDfmDoc::cloneDoc(type, (*itr)->getDocid());
			//old_doc->setBodySeqOff((*itr)->getBodySeqno(), (*itr)->getBodyOffset());
			AosDfmDocPtr old_doc = (*itr)->copyDoc();
			old_doc->setOpr(AosDfmDoc::eDelete);
			delete_docs.insert(old_doc);

			(*itr)->setBodySeqOff(0, 0);
			(*itr)->setOpr(AosDfmDoc::eNew);
			new_docs.insert((*itr));
		}
		mod_num++;
	}
	int del_num = delete_docs.size();
	int new_num = new_docs.size();

	if (del_sitr != docs.end())
	{
		if (add_sitr != docs.end())
		{
			// inser[start, end)
			delete_docs.insert(del_sitr, add_sitr);
		}
		else
		{
			delete_docs.insert(del_sitr, docs.end());
		}
	}

	if (add_sitr != docs.end()) new_docs.insert(add_sitr, docs.end());
	int del_num1 = delete_docs.size() - del_num;
	int new_num1 = new_docs.size() - new_num;
	int docs_num = docs.size();
	aos_assert_r(del_num1 + new_num1 + mod_num == docs_num, false);
	return true;
}


bool
AosDfmBody::addBodys(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_fs &docs)
{
	bool rslt = false;
	AosDfmCompareFunType::setitr_fs itr;
	for(itr = docs.begin(); itr != docs.end(); ++itr)
	{
		rslt = findBlock(rdata, dfm, *itr);	
		aos_assert_r(rslt, false);
	}

	//sort
	AosDfmCompareFunType::set_fs vv;             
	vv.insert(docs.begin(), docs.end());             
	aos_assert_r(docs.size() == vv.size(), false); 

	rslt = mSlabBlockMgr->addDocs(rdata, dfm, vv);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDfmBody::removeBodys(
		const AosRundataPtr &rdata, 
		AosDfmCompareFunType::set_fs &docs)
{
	bool rslt = false;
	AosDfmCompareFunType::setitr_fs itr;
	for(itr = docs.begin(); itr != docs.end(); ++itr)
	{
		rslt = removeBody(rdata, *itr);	
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosDfmBody::compressBody(
		AosBuffPtr &body_buff,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	aos_assert_rr(body_buff && body_buff->dataLen() > 0, rdata, false);
	u64 destLen = 0;
	AosBuffPtr dest_buff;
	bool rslt = AosCompress::compressStatic(
		smCompressAlg, dest_buff, destLen, body_buff,
		body_buff->dataLen(), rdata AosMemoryCheckerFileLine);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(dest_buff && dest_buff->dataLen() > 0 && (u32)dest_buff->dataLen() == destLen, rdata, false);
	aos_assert_rr(body_buff->dataLen() > dest_buff->dataLen(), rdata, false);
	body_buff = dest_buff;
	return true;
}


bool
AosDfmBody::uncompressBody(
		AosBuffPtr &body_buff,
		const u32 orig_len,
		const u32 compress_len,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	if(compress_len == 0)
	{
		// means this doc does't compress yet.
		aos_assert_r((u32)body_buff->dataLen() == orig_len, false);
		return true;
	}
	
	aos_assert_r((u32)body_buff->dataLen() == compress_len, false);
	AosBuffPtr buff;
	bool rslt = AosCompress::uncompressStatic(
		smCompressAlg, buff, orig_len, body_buff,
		compress_len, rdata AosMemoryCheckerFileLine);
	aos_assert_r(rslt && buff, false);
	body_buff = buff;
	return true;
}


void
AosDfmBody::print(
		const OmnString &name,
		AosDfmCompareFunType::set_fs &docs)
{
	if (!smShowLogBody) return;
	AosDfmCompareFunType::setitr_fs itr;
	for(itr = docs.begin(); itr != docs.end(); ++itr)
	{
		AosDfmDocPtr doc = *itr;
		OmnString docstr;
		docstr << name <<" opr: " << doc->getOpr() 
				<< "; docid:" << doc->getDocid()
				<<";seqno:" << doc->getBodySeqno() 
				<< ";offset:" << doc->getBodyOffset();
		if (doc->getOpr() != AosDfmDoc::eDelete)
				docstr <<";size:" << doc->getBodyBuff()->dataLen();
		OmnScreen << docstr <<endl;
	}
}


bool
AosDfmBody::flushBitmap(const AosRundataPtr &rdata)
{
	// Ketty 2014/03/25
	return mSlabBlockMgr->flushBitmap(rdata);
}


/*
bool
AosDfmBody::addBody(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		const AosDfmDocPtr &doc)
{
	AosBuffPtr body_buff = doc->getBodyBuff();
	u32 seqno = 0;
	u64 offset = 0;

	u32 orig_len = body_buff->dataLen();
	u32 compress_len = 0;
	if(doc->needCompress())
	{
		bool rslt = compressBody(body_buff, rdata AosMemoryCheckerArgs);
		aos_assert_r(rslt && body_buff, false);
		compress_len = body_buff->dataLen();
	}

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; add:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << doc->getDocid()
			<< "; orig_len:" << orig_len
			<< "; compress_len:" << compress_len
			<< "; read_only:" << (doc->readOnly() ? "true" : "false")
			<< endl;
	}

	bool isReadOnly = doc->readOnly();
	if (isReadOnly)
	{
		bool rslt = mReadOnlyBlock->addDoc(rdata, dfm, seqno, offset,
				body_buff->data(), orig_len, compress_len);
		aos_assert_r(rslt && (seqno !=0 || offset !=0), false);
		doc->resetBodySeqOff(seqno, offset);
		return true;
	}

	u32 data_len = body_buff->dataLen();
	bool is_big = AosDfmUtil::isBigData(data_len);
	if(is_big)
	{
		bool rslt = mSlabBlockMgr->addDoc(dfm, seqno, offset,
				orig_len, compress_len, body_buff->data(), rdata);
		aos_assert_r(rslt && (seqno !=0 || offset !=0), false);
		doc->resetBodySeqOff(seqno, offset);
		return true;
	}

	bool rslt = mDiskBlockMgr->addDoc(rdata, dfm, seqno, offset,
			orig_len, compress_len, body_buff->data());
	aos_assert_r(rslt && (seqno !=0 || offset !=0), false);
	doc->resetBodySeqOff(seqno, offset);
	return true;
}


bool
AosDfmBody::modifyBody(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		bool &kickout,
		const AosDfmDocPtr &doc)
{
	AosBuffPtr body_buff = doc->getBodyBuff();
	aos_assert_r(body_buff, false);
	u32 seqno = doc->getBodySeqno();
	u64 offset = doc->getBodyOffset();
	aos_assert_r(seqno !=0 || offset != 0, false);
	bool rslt = false;
	u32 orig_len = body_buff->dataLen();
	u32 compress_len = 0;
	
	if(doc->needCompress())
	{
		rslt = compressBody(body_buff, rdata AosMemoryCheckerArgs);
		aos_assert_r(rslt && body_buff, false);
		compress_len = body_buff->dataLen(); 
	}
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; modify:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << doc->getDocid()
			<< "; bodySeqno:" << seqno
			<< "; bodyOffset:" << offset
			<< "; orig_len:" << orig_len
			<< "; compress_len:" << compress_len
			<< endl;
	}

	kickout = false;
	bool orig_is_big = isInSlabFile(seqno);
	if(orig_is_big)
	{
		rslt = mSlabBlockMgr->modifyDoc(seqno, offset, orig_len, 
				compress_len, body_buff->data(), kickout, rdata);
	}
	else
	{
		rslt = mDiskBlockMgr->modifyDoc(rdata, seqno, offset,
				orig_len, compress_len, body_buff->data(), kickout);
	}
	aos_assert_r(rslt, false);

	if(kickout)
	{
		rslt = addBody(rdata, dfm, doc);
		aos_assert_r(rslt, false);
	}
	
	return true;
}


bool
AosDfmBody::removeBody(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	u32 seqno = doc->getBodySeqno();
	u64 offset = doc->getBodyOffset();
	bool rslt = false;
	
	bool isReadOnly = ((seqno & 0x40000000) == 0x40000000);
	aos_assert_r(!isReadOnly, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; remove:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mDfmId
			<< "; docid:" << doc->getDocid()
			<< "; bodySeqno:" << seqno
			<< "; bodyOffset:" << offset
			<< endl;
	}


	bool orig_is_big = isInSlabFile(seqno);
	if(orig_is_big)
	{
		rslt = mSlabBlockMgr->removeDoc(seqno, offset, rdata);	
		aos_assert_r(rslt, false);
		return true;
	}

	rslt = mDiskBlockMgr->removeDoc(rdata, seqno, offset);	
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDfmBody::readBody(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	u32 seqno = doc->getBodySeqno();
	u64 offset = doc->getBodyOffset();
	if(seqno == 0 && offset ==0) return true;
	
	bool rslt = false;
	AosBuffPtr data_buff;

	u32 orig_len = 0;
	u32 compress_len = 0;

	bool isReadOnly = ((seqno & 0x40000000) == 0x40000000);
	if (isReadOnly)
	{
		u32 rseqno = seqno & 0xbfffffff;
		rslt = mReadOnlyBlock->readDoc(rdata, rseqno, offset, data_buff, orig_len, compress_len);
		//aos_assert_r(rslt, false);
		if(!rslt)
		{
			OmnAlarm << "ReadOnly Error!" << orig_len
				<< "; docid:" << doc->getDocid()
				<< "; seqno:" << seqno
				<< "; offset:" << offset
				<< enderr;
			return false;
		}

		
		rslt = uncompressBody(data_buff, orig_len, compress_len, rdata AosMemoryCheckerArgs);
		aos_assert_rr(rslt && data_buff, rdata, false);

		doc->setBodyBuff(data_buff);
		return true;
	}

	bool is_big = isInSlabFile(seqno);
	if(is_big)
	{
		rslt = mSlabBlockMgr->readDoc(seqno, offset, data_buff, orig_len, compress_len, rdata);
		aos_assert_rr(rslt, rdata, false);
		
		rslt = uncompressBody(data_buff, orig_len, compress_len, rdata AosMemoryCheckerArgs);
		aos_assert_rr(rslt && data_buff, rdata, false);

		doc->setBodyBuff(data_buff);
		return true;
	}

	rslt = mDiskBlockMgr->readDoc(rdata, seqno, offset, data_buff, orig_len, compress_len);
	aos_assert_rr(rslt, rdata, false);
	
	rslt = uncompressBody(data_buff, orig_len, compress_len, rdata AosMemoryCheckerArgs);
	aos_assert_rr(rslt && data_buff, rdata, false);
	
	doc->setBodyBuff(data_buff);
	return true;
}


bool
AosDfmBody::isInSlabFile(const u32 fseqno)
{
	return ((fseqno & 0x80000000) == 0x80000000);
}

	
bool
AosDfmBody::saveSlabFileBitmap(const u32 fseqno, const AosRundataPtr &rdata)
{
	u32 rseqno = fseqno & 0x7fffffff;
	return mSlabBlockMgr->saveBitmap(rseqno, rdata);
}
*/


