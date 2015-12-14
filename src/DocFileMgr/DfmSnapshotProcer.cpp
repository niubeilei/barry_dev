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
// 2014/02/26 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
//
#include "DocFileMgr/DocFileMgr.h"

#include "DfmUtil/DfmDocHeader.h"
#include "DocFileMgr/DfmHeader.h"
#include "DocFileMgr/DfmBody.h"
#include "FmtMgr/Fmt.h"
#include "SEInterfaces/DfmSnapshotType.h"
#include "SEInterfaces/DfmSnapshotObj.h"
#include "SEInterfaces/FmtMgrObj.h"
#include "API/AosApi.h"

// SnapshotProcer
// 
u64
AosDocFileMgr::createSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosTransId &trans_id)	
{
	aos_assert_r(mSnapshotMgr, 0);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; createSnapshot;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	AosFmtPtr fmt = createFmt(rdata);
	aos_assert_r(fmt, 0);

	u64 new_snap_id = mSnapshotMgr->createSnapshot(
			rdata, snap_id, AosDfmSnapshotType::eBigData);
	aos_assert_r(new_snap_id, 0);
	if(!new_snap_id)
	{
		OmnAlarm << "create Snapshot error!" << enderr;
	}

	vector<AosTransId> tids;
	tids.push_back(trans_id);
	fmt->add(tids);
	sendFmt(rdata, fmt);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; createSnapshot finish;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; snap_id:" << new_snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	return new_snap_id;
}


bool
AosDocFileMgr::commitSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosTransId &trans_id)
{
	aos_assert_r(mSnapshotMgr, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; commitSnapshot;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; snap_id:" << snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	AosFmtPtr fmt = createFmt(rdata);
	aos_assert_r(fmt, false);
	
	bool rslt = mSnapshotMgr->commitSnapshot(rdata, snap_id);
	if(!rslt)
	{
		OmnAlarm << "commit Snapshot error!"
			<< " snap_id:" << snap_id
			<< enderr;
	}
	
	vector<AosTransId> tids;
	tids.push_back(trans_id);
	fmt->add(tids);
	sendFmt(rdata, fmt);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; commitSnapshot finish;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; snap_id:" << snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}

	return true;
}


bool
AosDocFileMgr::rollbackSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosTransId &trans_id)
{
	aos_assert_r(mSnapshotMgr, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; rollbackSnapshot;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; snap_id:" << snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	AosFmtPtr fmt = createFmt(rdata);
	aos_assert_r(fmt, false);
	
	bool rslt = mSnapshotMgr->rollbackSnapshot(rdata, snap_id);
	if(!rslt)
	{
		OmnAlarm << "rollback Snapshot error!"
			<< " snap_id:" << snap_id
			<< enderr;
	}
	
	vector<AosTransId> tids;
	tids.push_back(trans_id);
	fmt->add(tids);
	sendFmt(rdata, fmt);

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; rollbackSnapshot finish;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; snap_id:" << snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	return true;
}


bool
AosDocFileMgr::mergeSnapshot(
		const AosRundataPtr &rdata,
		const u64 target_snap_id,
		const u64 merger_snap_id,
		const AosTransId &trans_id)
{
	aos_assert_r(mSnapshotMgr, false);
	
	//if(mShowLog)
	{
		OmnScreen << "DocFileMgr; mergeSnapshot;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; target_snap_id:" << target_snap_id 
			<< "; merger_snap_id:" << merger_snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	AosFmtPtr fmt = createFmt(rdata);
	aos_assert_r(fmt, false);
	
	bool rslt = mSnapshotMgr->mergeSnapshot(rdata, target_snap_id, merger_snap_id);
	if(!rslt)
	{
		OmnAlarm << "merge Snapshot error!"
			<< " target_snap_id:" << target_snap_id
			<< " merger_snap_id:" << merger_snap_id
			<< enderr;
	}
	
	vector<AosTransId> tids;
	tids.push_back(trans_id);
	fmt->add(tids);
	sendFmt(rdata, fmt);
	
	//if(mShowLog)
	{
		OmnScreen << "DocFileMgr; mergeSnapshot finish;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; target_snap_id:" << target_snap_id 
			<< "; merger_snap_id:" << merger_snap_id 
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	
	return true;
}


AosDfmDocPtr
AosDocFileMgr::readDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const u64 docid,
		const bool read_body)
{
	if (!snap_id) return readDoc(rdata, docid, read_body);
	
	aos_assert_r(mSnapshotMgr, 0);

	//return mSnapshotMgr->readDoc(rdata, snap_id, docid, read_body);
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; readDoc use snap;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; snap_id:" << snap_id
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << docid
			<< "; read_body:" << read_body
			<< endl;
	}
	
	//AosDfmSnapshotObjPtr snapshot = mSnapshotMgr->getSnapshotById(snap_id);
	//aos_assert_r(snapshot, 0);

	bool exist = false;
	AosDfmDocPtr doc = readDocFromSnapshot(rdata, docid, read_body, snap_id, exist);
	if(exist)
	{
		aos_assert_r(doc, 0);
		return doc;
	}

	// if come here means snapshot don't has this doc. read from Dfm.
	return readDocFromDfm(rdata, docid, read_body);	
}


bool
AosDocFileMgr::saveDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
	if (!snap_id) return saveDoc(rdata, trans_ids, doc); 

	aos_assert_r(mSnapshotMgr, false);
	bool rslt;
	
	// flush dfmDoc's mem data to mHeaderBuff. for IIL.
	doc->flushToHeaderBuff();

	// compress.
	AosBuffPtr body_buff = doc->getBodyBuff();
	u32 orig_len = body_buff->dataLen();
	u32 compress_len = 0;
	if(doc->needCompress())
	{	
		rslt = mBody->compressBody(body_buff, rdata AosMemoryCheckerArgs);
		aos_assert_r(rslt && body_buff, false);
		compress_len = body_buff->dataLen(); 
		doc->setBodyBuff(body_buff);    // reset the compressed body.
	}
	doc->setOrigLen(orig_len);
	doc->setCompressLen(compress_len);

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; saveDoc use snap;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; snap_id:" << snap_id
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << doc->getDocid()
			<< endl;
	}

	AosFmtPtr fmt = createFmt(rdata);
	aos_assert_r(fmt, 0);

	AosDfmSnapshotObjPtr snapshot = mSnapshotMgr->getSnapshotById(snap_id);
	aos_assert_r(snapshot, false);
    
	OmnMutexPtr lock = mLockPool.get(doc->getDocid(), smLockGen);
	lock->lock();
	
	rslt = saveDocSnapKeepNew(rdata, doc, snapshot);
	aos_assert_rl(rslt, lock, false);
	
	lock->unlock();

	// what about trans_id?
	fmt->add(trans_ids);
	sendFmt(rdata, fmt);
	return true;
}


bool
AosDocFileMgr::deleteDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
	if (!snap_id) return deleteDoc(rdata, trans_ids, doc);

	aos_assert_r(mSnapshotMgr, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; deleteDoc use snap;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; snap_id:" << snap_id
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << doc->getDocid()
			<< endl;
	}
	
	bool rslt;
	AosFmtPtr fmt = createFmt(rdata);
	aos_assert_r(fmt, 0);
	
	AosDfmSnapshotObjPtr snapshot = mSnapshotMgr->getSnapshotById(snap_id);
	aos_assert_r(snapshot, false);
	
	OmnMutexPtr lock = mLockPool.get(doc->getDocid(), smLockGen);
	lock->lock();

	// what about trans_id?
	rslt = deleteDocSnapKeepNew(rdata, doc, snapshot);
	aos_assert_rl(rslt, lock, false);
	
	lock->unlock();

	fmt->add(trans_ids);
	sendFmt(rdata, fmt);
	return true;
}


AosDfmDocPtr
AosDocFileMgr::readDocFromSnapshot(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body,
		const u64 snap_id,
		bool &exist)
{
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; readDoc from snap;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; snap_id:" << snap_id
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << docid
			<< "; read_body:" << read_body
			<< endl;
	}
	
	AosDfmDocHeaderPtr crt_header;
	//bool rslt = snapshot->readEntry(rdata, docid, exist, crt_header);
	bool rslt = mSnapshotMgr->readEntry(rdata, docid, snap_id, exist, crt_header); 
	aos_assert_r(rslt, 0);
	if(!exist)	return 0;
	
	aos_assert_r(crt_header, 0);
	AosDfmDocPtr doc = AosDfmDoc::cloneDoc(mConfig.mDocType, crt_header);
	if(read_body)
	{
		rslt = mBody->readBody(rdata, doc);
		aos_assert_r(rslt, 0);
	}
	return doc;
}


bool
AosDocFileMgr::saveDocSnapKeepNew(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(snapshot, false);
	AosDocFileMgrObjPtr thisptr(this, false);

	bool rslt, exist;
	AosDfmDocHeaderPtr crt_header, new_header;
	rslt = snapshot->readEntry(rdata, doc->getDocid(), exist, crt_header);
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; saveDoc from snap;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; snap_id:" << snapshot->getSnapId()
			<< "; snap_id:" << snapshot->getSnapId()
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << doc->getDocid() 
			<< "; exist:" << exist
			<< "; "
			<< endl;
	}
	
	if(exist)
	{
		aos_assert_r(crt_header, false);
		doc->setBodySeqOff(crt_header->getBodySeqno(),
				crt_header->getBodyOffset());
		rslt = saveBody(rdata, doc);
		aos_assert_r(rslt, false);
		
		new_header = doc->getHeader();
		// iil will always save the header.
		//if(new_header->isSameBodySeqOff(crt_header))	return true;

		rslt = snapshot->addEntry(rdata, new_header);
		aos_assert_r(rslt, false);
		return true;
	}

	// this crt_header status maybe deleted.
	doc->setBodySeqOff(0, 0);
	rslt = mBody->addBody(rdata, thisptr, doc);
	aos_assert_r(rslt, false);

	new_header = doc->getHeader();
	rslt = snapshot->addEntry(rdata, new_header);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocFileMgr::deleteDocSnapKeepNew(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(doc && snapshot, false);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; deleteDoc from snap;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; snap_id:" << snapshot->getSnapId()
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << doc->getDocid() 
			<< "; "
			<< endl;
	}
	
	bool exist;
	AosDfmDocHeaderPtr crt_header;
	bool rslt = snapshot->readEntry(rdata, doc->getDocid(), exist, crt_header);
	aos_assert_r(rslt, false);
	
	if(exist)
	{
		// normal delete Doc.
		aos_assert_r(crt_header, false);
		if(crt_header->isEmpty())
		{
			// means this doc has deleted. this has marked in snapshot
			OmnScreen << "DocFileMgr; this doc has deleted yet!"
				<< "; virtual_id:" << mVirtualId
				<< "; dfm_id:" << mId
				<< "; snap_id:" << snapshot->getSnapId()
				<< "; prefix:" << mConfig.mPrefix
				<< "; docid:" << doc->getDocid() 
				<< "; "
				<< endl;
			return true;
		}

		doc->setBodySeqOff(crt_header->getBodySeqno(),
				crt_header->getBodyOffset());
		rslt = mBody->removeBody(rdata, doc);
		aos_assert_r(rslt, false);

		rslt = snapshot->removeEntry(rdata, doc->getDocid());
		aos_assert_r(rslt, false);
		return true;
	}

	// mark doc delete.  this new_header is empty.
	AosDfmDocHeaderPtr new_header = doc->cloneDfmHeader(doc->getDocid());
	rslt = snapshot->addEntry(rdata, new_header);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocFileMgr::commit(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(snapshot, 0);	
	bool rslt;
	
	vector<AosDfmDocHeaderPtr> entrys;
	AosSnapshotReadInfo read_info;
	while(1)
	{
		entrys.clear();
		rslt = snapshot->getEntrys(rdata, entrys, read_info);
		aos_assert_r(rslt, false);	
		if(read_info.finished)	break;
		
		for(u32 i=0; i<entrys.size(); i++)
		{
			rslt = flushSnapshotToDfm(rdata, entrys[i]);
			aos_assert_r(rslt, false);
		}
	}
	
	mBody->flushBitmap(rdata);
	return true;
}


bool
AosDocFileMgr::rollback(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(snapshot, 0);	
	bool rslt;
	
	vector<AosDfmDocHeaderPtr> entrys;
	AosSnapshotReadInfo read_info;
	while(1)
	{
		rslt = snapshot->getEntrys(rdata, entrys, read_info);
		aos_assert_r(rslt, false);	
		if(read_info.finished)	break;
		
		for(u32 i=0; i<entrys.size(); i++)
		{
			rslt = cleanSnapshotFromDfm(rdata, entrys[i]);
			aos_assert_r(rslt, false);
		}
	}

	mBody->flushBitmap(rdata);
	return true;
}


bool
AosDocFileMgr::merge(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &target_snapshot,
		const AosDfmSnapshotObjPtr &merger_snapshot)
{
	aos_assert_r(target_snapshot && merger_snapshot, false);

	bool rslt;
	vector<AosDfmDocHeaderPtr> entrys;
	AosSnapshotReadInfo read_info;
	while(1)
	{
		rslt = merger_snapshot->getEntrys(rdata, entrys, read_info);
		aos_assert_r(rslt, false);	
		if(read_info.finished)	break;
	
		for(u32 i=0; i<entrys.size(); i++)
		{
			rslt = flushEntryToSnapshot(rdata, target_snapshot, entrys[i]);
			aos_assert_r(rslt, false);
		}
	}

	mBody->flushBitmap(rdata);
	return true;
}


bool
AosDocFileMgr::flushSnapshotToDfm(
		const AosRundataPtr &rdata,
		const AosDfmDocHeaderPtr &snap_header)
{
	// this entrys is from snapshot.

	aos_assert_r(snap_header, false);

	bool rslt;	
	AosDocFileMgrObjPtr thisptr(this, false);
	AosDfmDocHeaderPtr crt_header;

	rslt = mHeader->readHeader(rdata, snap_header->getDocid(), crt_header);
	aos_assert_r(rslt && crt_header, false);
		
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; flush snapshot; DfmLog;" 
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; crt_docid:" << crt_header->getDocid()
			<< "; crt_bodySeqno:" << crt_header->getBodySeqno()
			<< "; crt_bodyOffset:" << crt_header->getBodyOffset()
			<< "; snap bodySeqno:" << snap_header->getBodySeqno()
			<< "; bodyOffset:" << snap_header->getBodyOffset()
			<< endl;
	}

	//if(crt_header->isSameBodySeqOff(snap_header))	continue;
	
	// isSameBodySeqOff maybe flush two times. but also need saveHeader.
	// because need generate Fmt to bkp svr.
	if(!crt_header->isSameBodySeqOff(snap_header) && !crt_header->isEmpty())
	{
		rslt = mBody->removeBody(rdata, crt_header);
		aos_assert_r(rslt, false);
	}

	rslt = mHeader->saveHeader(rdata, thisptr, snap_header);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDocFileMgr::cleanSnapshotFromDfm(
		const AosRundataPtr &rdata,
		const AosDfmDocHeaderPtr &snap_header)
{
	// this entrys is from snapshot.
	aos_assert_r(snap_header, false);

	if(snap_header->isEmpty())	return true;	
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; clean snapshot; DfmLog;" 
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; snap_docid:" << snap_header->getDocid()
			<< "; snap bodySeqno:" << snap_header->getBodySeqno()
			<< "; bodyOffset:" << snap_header->getBodyOffset()
			<< endl;
	}

	// maybe clean two times.
	bool rslt = mBody->removeBody(rdata, snap_header);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDocFileMgr::flushEntryToSnapshot(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &target_snapshot,
		const AosDfmDocHeaderPtr &new_entry)
{
	aos_assert_r(target_snapshot && new_entry, false);	
	
	bool exist;
	AosDfmDocHeaderPtr crt_entry;
	bool rslt = target_snapshot->readEntry(rdata, new_entry->getDocid(), exist, crt_entry);
	aos_assert_r(rslt, false);
	
	//if(mShowLog)
	{
		OmnString str = "DocFileMgr; flush entry to snapshot; DfmLog;"; 
		str << "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; target_snap_id:" << target_snapshot->getSnapId()
			<< "; exist:" << exist 
			<< "; crt_docid:" << new_entry->getDocid();
		if(exist)
		{
			str << "; crt_bodySeqno:" << crt_entry->getBodySeqno()
				<< "; crt_bodyOffset:" << crt_entry->getBodyOffset();
		}
		OmnScreen << str << endl;
	}

	if(exist)
	{
		aos_assert_r(crt_entry, false);
	
		// Ketty 2014/03/26 only for test.
		//OmnAlarm << "maybe error" << enderr;

		// isSameBodySeqOff maybe flush two times. but also need saveHeader.
		// because need generate Fmt to bkp svr.
		if(!crt_entry->isSameBodySeqOff(new_entry) && !crt_entry->isEmpty())
		{
			AosDfmDocPtr doc = AosDfmDoc::cloneDoc(mConfig.mDocType);
			doc->setBodySeqOff(crt_entry->getBodySeqno(),
					crt_entry->getBodyOffset());
			rslt = mBody->removeBody(rdata, doc);
			aos_assert_r(rslt, false);
		}
	}
	
	rslt = target_snapshot->addEntry(rdata, new_entry, true);
	aos_assert_r(rslt, false);
	return true;	
}


AosFmtPtr
AosDocFileMgr::createFmt(const AosRundataPtr &rdata)
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, 0);
	
	AosFmtPtr fmt = OmnNew AosFmt();
	fmt_mgr->setFmt(fmt);
	return fmt;	
}


bool
AosDocFileMgr::sendFmt(
		const AosRundataPtr &rdata,
		const AosFmtPtr &fmt)
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);
	
	fmt_mgr->sendFmt(rdata, fmt);
	fmt_mgr->setFmt(0);

	// Ketty temp.
	vector<AosTransId>  trans_ids = fmt->getTransIds();
	AosFinishTrans(trans_ids);
	
	return true;	
}



/*
bool
AosDocFileMgr::saveDocSnapKeepOrig(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(snapshot, false);
	AosDocFileMgrObjPtr thisptr(this, false);

	bool exist;
	bool rslt = snapshot->entryExist(rdata, doc->getDocid(), exist);
	aos_assert_r(rslt, false);
	
	if(exist)
	{
		// normal save Doc.
		rslt = saveDocToDfm(rdata, doc);
		aos_assert_r(rslt, false);
		return true;
	}

	AosDfmDocHeaderPtr orig_header;
	rslt = mHeader->readHeader(rdata, doc->getDocid(), orig_header);
	aos_assert_r(rslt && orig_header, false);

	rslt = snapshot->addEntry(rdata, orig_header);
	aos_assert_r(rslt, false);
	
	rslt = mBody->addBody(rdata, thisptr, doc);
	aos_assert_r(rslt, false);

	AosDfmDocHeaderPtr new_header = doc->getHeader();
	rslt = mHeader->saveHeader(rdata, thisptr, new_header);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDocFileMgr::deleteDocSnapKeepOrig(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		const AosDfmSnapshotObjPtr &snapshot)
{
	OmnShouldNeverComeHere;
	return false;
	
	aos_assert_r(doc && snapshot, false);
	AosDocFileMgrObjPtr thisptr(this, false);
	
	bool exist;
	bool rslt = snapshot->entryExist(rdata, doc->getDocid(), exist);
	aos_assert_r(rslt, false);
	
	if(exist)
	{
		rslt = deleteDocFromDfm(rdata, doc);
		aos_assert_r(rslt, false);
		return true;
	}

	AosDfmDocHeaderPtr orig_header; 
	rslt = mHeader->readHeader(rdata, doc->getDocid(), orig_header);
	aos_assert_r(rslt && orig_header, false);

	if(orig_header->isEmpty()) return true; 	// means this doc not exist.
	
	rslt = snapshot->addEntry(rdata, orig_header);
	aos_assert_r(rslt, false);
	
	AosDfmDocHeaderPtr new_header = doc->cloneDfmHeader(doc->getDocid());
	rslt = mHeader->saveHeader(rdata, thisptr, new_header);		// this new_header is empty.
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDocFileMgr::commit(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &snapshot,
		const bool snap_keepnew)
{
	aos_assert_r(snapshot, 0);	
	bool rslt;
	
	vector<AosDfmDocHeaderPtr> entrys;
	AosSnapshotReadInfo read_info;
	while(1)
	{
		rslt = snapshot->getEntrys(rdata, entrys, read_info);
		aos_assert_r(rslt, false);	
		if(read_info.finished)	break;
		
		if(snap_keepnew)
		{
			rslt = flushSnapshotToDfm(rdata, entrys);
		}
		else
		{
			rslt = cleanSnapshotFrmDfm(rdata, entrys);
		}
		aos_assert_r(rslt, false);
	}
	
	return true;
}


bool
AosDocFileMgr::rollback(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &snapshot,
		const bool snap_keepnew)
{
	aos_assert_r(snapshot, 0);	
	bool rslt;
	
	vector<AosDfmDocHeaderPtr> entrys;
	AosSnapshotReadInfo read_info;
	while(1)
	{
		rslt = snapshot->getEntrys(rdata, entrys, read_info);
		aos_assert_r(rslt, false);	
		if(read_info.finished)	break;
		
		if(snap_keepnew)
		{
			rslt = cleanSnapshotFrmDfm(rdata, entrys);
		}
		else
		{
			rslt = flushSnapshotToDfm(rdata, entrys);
		}
		aos_assert_r(rslt, false);
	}

	return true;
}

*/

