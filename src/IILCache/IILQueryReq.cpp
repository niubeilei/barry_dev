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
//
// Modification History:
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/IILQueryReq.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

AosIILQueryReq::AosIILQueryReq(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	if (!initQuery(rdata, ms))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosIILQueryReq::~AosIILQueryReq()
{
}


bool
AosIILQueryReq::isFirstBlock() const
{
	return (mOrigStartPos == mStartPos);
}


void
AosIILQueryReq::setCrtStartPos(const AosIILIdx &start_pos)
{
	mCrtStartPos = start_pos;
}


void
AosIILQueryReq::setOrigStartPos(const AosIILIdx &start_pos)
{
	mOrigStartPos = start_pos;
}


void
AosIILQueryReq::setEndPos(const AosIILIdx &start_pos)
{
	mEndStartPos = end_pos;
}


bool 
AosIILQueryReq::isEmpty() const
{
	return mCrtStartPos > mEndPos;
}


bool
AosIILQueryReq::isFinished() const
{
	if (!mCrtStartPos.isFinished(mEndPos, mReverse)) return false;
	if (mCrtBlockSize > 0) return false;
	return true;
}


bool
AosIILQueryReq::initQuery(
		const AosRundataPtr &rdata, 
		const AosBuffMsg &msg)
{
}


AosCubeMsgProcPtr 
AosIILQueryReq::cloneCubeMsgProc()
{
	return OmnNew AosIILQueryReq();
}


bool
AosIILQueryReq::procMsg(
		const AosRundataPtr &rundata, 
		const AosBuffMsgPtr &msg)
{
	aos_assert_rr(msg, rundata, false);
	AosRundataPtr rdata = msg->getRundata();
	aos_assert_rr(rdata, rundata, false);

	OmnMsgId::E msgid = msg->getMsgId();
	switch (msgid)
	{
	case OmnMsgId::eRetrvBitmapNodesResp:
		 return procRetrvNodesResp(rdata, (AosMsgRetrvNodesResp*)msg.getPtr());

	case OmnMsgId::eSectionResults:
		 return procSectionResults(rdata, (AosMsgSectionResultsResp*)msg.getPtr());

	case OmnMsgId::eRetrvIILBlockResp:
		 return procRetrvIILBlockResp(rdata, (AosMsgRetrvIILBlockResp*)msg.getPtr());

	default:
		 OmnAlarm << "Unrecognized message: " << msgid << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILQueryReq::procPhase2Req(
		const AosRundataPtr &rdata, 
		const AosMsgPhase2ReqPtr &msg)
{
	// A message has been received. The message contains the bitmaps
	// and nodelists that are organized by sections. This function
	// needs to loop over all the sections. For each section, it checks
	// whether the bitmaps have been cached. If not, it needs to retrieve
	// the bitmaps from the remote server. 
	// Once all the bitmaps are retrieved, it is ready to calculate the
	// final results. Once the final results are calculated, it sends
	// the results to the sorting IIL, which will reassemble all the results
	// and send the results back to the requester.
	
	// Step 1
	// There are N condition IILs. For each IIL, there is a node list.
	// The node list determines which bitmaps to retrieve. For each bitmap
	// in the node list, it checks whether it is locally cached. It packs 
	// all the bitmaps that are not locally cached. After that, it sends a 
	// request to retrieve all the bitmaps.
	//
	// Step 2
	// For the sorting IIL, if it has node list, it is processed the same
	// way as condition IILs. 
	//
	// Step 3
	// Once all the bitmaps for all the IILs are retrieved, for each condition
	// IIL, it ORs the bitmaps into one. It then ANDs all the bitmaps. If it is
	// empty, there are no results for the section. Otherwise, it adds the 
	// results to the running results. 
	//
	// Step 4
	// When all sections finish the processing, the running results contains all
	// the bits that are picked by this physical machine. It sends the results to
	// the sorting IIL, which will assemble the final results.
	//
	// The message has two buffs: bitmap buff and nodelist buff. The bitmap
	// buff is in the following format:
	// 		section_id				(for sorting IIL)
	// 		iilname					(for sorting IIL)
	// 		bitmap buff				(for sorting IIL)
	// 		section_id				(for condition IIL)
	// 		iilname					(for condition IIL)
	// 		bitmap buff				(for condition IIL)
	// 		...
	// and the nodelist buff is in the following format:
	// 		section_id				(for sorting IIL)
	// 		iilname					(for sorting IIL)
	// 		nodelist buff			(for sorting IIL)
	// 		section_id				(for condition IIL)
	// 		iilname					(for condition IIL)
	// 		nodelist buff			(for condition IIL)
	// 		...

	AosBuffPtr bitmap_buff = msg->getBitmapBuff();
	if (!bitmap_buff)
	{
		OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
		queryFailed(rdata, errmsg);
		return false;
	}

	AosBuffPtr nodelist_buff = msg->getBitmapBuff();
	if (!nodelist_buff)
	{
		OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
		queryFailed(rdata, errmsg);
		return false;
	}

	AosBuffPtr req_buff = OmnNew AosBuff();
	u64 section_id = bitmap_buff->getU64(AOS_INV_SECID);
	while (section_id != AOS_INV_SECID)
	{
		procSection(rdata, section_id, bitmap_buff, nodelist_buff, req_buff);
	}

	if (req_buff->dataLen() > 0)
	{
		// It needs to retrieve.
		int recv_phyid = AosSectionId2PhysicalId(section_id);
		AosMsgProcPtr thisptr(this, false);
		AosMsgRetrvNodesPtr msg = OmnNew AosMsgRetrvNodes(rdata, 
				smSelfPhysicalId, recv_phyid, req_buff);
		bool rslt = AosSendCubeMsg(rdata, msg, thisptr);
		if (!rslt)
		{
			queryFailed(rdata, rdata->getErrmsg());
			rturn false;
		}
	}
	else
	{
		// This means that all the needed bitmaps are locally cached.
		bool rslt = computeLocalResults(rdata);
		aos_assert_rr(rslt, rdata, false);
	}

	return true;
}


AosBitmapObjPtr
AosIILQueryReq::getFrontBitmap(const AosRundataPtr &rdata)
{
	mNodeIdx = -1;
	return mFrontBitmap;
}


AosBitmapObjPtr
AosIILQueryReq::getTailBitmap(const AosRundataPtr &rdata)
{
	return mEndBitmap;
}


AosBuffArrayPtr
AosIILQueryReq::getFrontDocids(const AosRundataPtr &rdata)
{
	// It retrieves the docids from the first IIL. It can be
	// partial docids.
	
	// If it is to retrieve a whole IIL, use 'cache_engine'
	// to retrieve since it may be cached.
	if (mStartIdx.getIdx(0) > 0 || 
		(mEndIdx.getIdx(0) >= 0 && mStartIILID == mEndIILID))
	{
		// This is a partial IIL.
		AosBuffArrayPtr buffarray = AosRetrieveIILDocids(
				rdata, mStartIILID, mStartIdx, mEndIdx);
		aos_assrt_rr(buffarray, rdata, false);
		return buffarray;
	}

	return AosGetIILCacheEngine()->getDocids(rdata, mStartIILID);
}


AosBuffArrayPtr
AosIILQueryReq::getEndDocids(const AosRundataPtr &rdata)
{
	// It retrieves the docids from the last IIL. It can be
	// partial docids.
	
	// The start and end IILs should not be the same IIL
	// because if it is the same IIL, it should have been 
	// retrieved by "getFrontDocids(...)".
	if (mStartIILID == mEndIILID)
	{
		return false;
	}
	
	// If it is to retrieve a whole IIL, use 'cache_engine'
	// to retrieve since it may be cached.
	if (mEndIdx.getIdx(0) >= 0)
	{

		// This is a partial IIL.
		AosBuffArrayPtr buffarray = AosRetrieveIILDocids(
				rdata, mEndIILID, mStartIdx, mEndIdx);
		aos_assrt_rr(buffarray, rdata, false);
		return buffarray;
	}

	return AosGetIILCacheEngine->getDocids(rdata, mEndIILID);
}


AosBitmapObjPtr
AosIILQueryReq::nextBitmap(
		const AosRundataPtr &rdata, 
		u64 &iilid, 
		int &level)
{
	// This function retrieves the next node.
	// Nodes are stored in mNodes.
	mNodeIdx++;
	if (mNodeIdx >= 0 && (u32)mNodeIdx < mNodes.size())
	{
		Node &node = mNodes[mNodeIdx];
		iilid = node.iilid;
		level = node.level;
		u64 bitmap_id = node.bitmap_id;
		return AosGetBitmapStorageEngine()->retrieveBitmap(rdata, bitmap_id);
	}

	return 0;
}


AosBitmapObjPtr
AosIILQueryReq::getLeafBitmap(
		const AosRundataPtr &rdata, 
		u64 &iilid, 
		const int idx)
{
	// This function retrieves the 'idx'-th bitmap of the current
	// node, identified by 'mNodeIdx'. 
	aos_assert_rr(mNodeIdx >= 0, rdata, 0);
	aos_assert_rr((u32)mNodeIdx < mNodes.size(), rdata, 0);
	aos_assert_rr(idx >= 0, rdata, 0);

	Node &node = mNodes[mNodeIdx];
	level = node.level;
	aos_assert_rr(level == 1, rdata, 0);

	// If it is level 1, the node should contain the following:
	// 		iilid		u64
	// 		bitmap_id	u64
	// 		iilid		u64
	// 		bitmap_id	u64
	// 		...
	int nn = idx*2;
	if ((u32)nn >= node.children.size()) return 0;

	iilid = node.children[nn];
	u64 bitmap_id = node.children[nn+1];
	return AosGetBitmapStorageEngine()->retrieveBitmap(rdata, bitmap_id);
}


bool
AosIILQueryReq::processLeaf(
		const AosRundataPtr &rdata,
		const AosBuffArrayPtr &results,
		const AosBuffArrayPtr &docids,
		const AosBitmapObjPtr &bitmap)
{
	// This function retrieves the IIL, and then the docids from it.
	const u64 *dd = (u64 *)docids->data();
	u32 num_docids = docids->size();
	for (u32 i=0; i<num_docids; i++)
	{
		if (bitmap->check(dd[i]))
		{
			results->append(dd[i]);
		}
		bb->andBitmap(bitmap);
	}

	return true;
}


bool
AosIILQueryReq::queryFailed(
		const AosRundataPtr &rdata, 
		const AosIILQueryReqObjPtr &query,
		const OmnString &errid, 
		const OmnString &errmsg, 
		const char *fname,
		const int line)
{
	OmnAlarm << "Query failed: " << fname << ":" << line
		<< ": " << errid << ":" << errmsg << enderr;
	mLock->lock();
	u64 query_id = query->getQueryId();
	mQueryCache.erase(query_id);

	int physical_id = query->getSendPhysicalId();
	aos_assert_rr(AosIsValidPhysicalid(physical_id, rdata, false), rdata, false);
	AosBuffPtr buff = AosCreateBuffMsgHead(
			rdata, smSendPhysicalId, physical_id, AOSQUERYMSG_RET_QUERYBK_RESP);
	buff->setU32(AosMsgFieldId::eStatus);
	buff->setBool(false);
	sendMsg(rdata, buff);
	return true;
}


bool
AosIILQueryReq::retrieveQueryBlock(
		const AosRundataPtr &rdata, 
		const AosMsgRetrvQueryBlockPtr &msg)
{
	// A query is processed in query blocks. Each block is identified
	// by the condition and the start position of the block. It checks
	// whether the block is cached. If yes, it returns the cached.
	// Otherwise, it sends a message to the corresponding server to 
	// retrieve the block. After that, it locally caches the block 
	// and then sends the block back to the caller.
	//
	// A query block is defined:
	// 	1. A condition (will not change for the same query)
	// 	2. The start position (will change for the same query)
	// 	3. Front and End Partial bitmaps
	// 	4. Front and End Level 0 and 1 bitmaps
	// 	5. Nodelists
	
	aos_assert_fr(msg, rdata, false);
	mSenderPhyid = msg->getFieldInt(AosMsgFieldId::eSendPhysicalId, -1);
	aos_assert_fr(mSenderPhyid > 0, rdata, false);

	mIILName = msg->getIILName();
	mQueryId = msg->getQueryId();
	mContextBuff = msg->getContextBuff();
	mBlockSize = msg->getBlockSize();
	mBlockStartPos = msg->getBlockStartPos();

	aos_assert_fr(verifySenderPhysicalId(), rdata, false);
	aos_assert_fr(mIILName != "", rdata, false);
	aos_assert_fr(mQueryId > 0, rdata, false);
	aos_assert_fr(verifyQueryContext(), rdata, false);
	aos_assert_fr(verifyQueryBlockSize(), rdata, false);
	
	AosQueryContextObjPtr context = AosCreateQueryContext(rdata, mContextBuff);
	aos_assert_fr(context, rdata, false);

	// Now we have collected all the information. Check whether the 
	// block is locally cached.
	AosBuffPtr result_buff;
	bool cached;
	AosBuffPtr result_buff = AosGetIILCacheEngine()->getQueriedData(rdata, mIILName, 
			context, mBlockStartPos);
	if (result_buff)
	{
		rslt = sendQueryBlockResp(rdata, result_buff, msg);
		aos_assert_fr(rslt, rdata, false);
		return true;
	}

	// It was not cached. Need to retrieve the block.
	if (isIILLocal(mIILName))
	{
		mTimestampQueryCreated = OmnGetTimestamp();
		AosQueryReqObjPtr query(thisptr, false);
	
		// Ready to retrieve the query block.
		AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
		if (!iilmgr)
		{
			queryFailed(rdata, "missing_iilmgr", AosFileAndLine);
			return false;
		}

		// Retrieve the query block
		bool rslt = iilmgr->retrieveQueryBlock(rdata, mIILName, 
				context, mBlockStartPos, result_buff);
		aos_assert_fr(rslt, rdata, false);

		if (result_buff)
		{
			// Query block is retrieved. Need to cache the block.
			bool rslt = AosGetIILCacheEngine()->cacheQueriedData(rdata, 
					mIILName, context, mBlockStartPos, result_buff); 
			aos_assert_fr(rslt, rdata, false);
		}

		// Send the response.
		bool rslt = sendQueryBlockResp(rdata, result_buff, msg);
		aos_assert_fr(rslt, rdata, false);
		return true;
	}

	// The IIL is not local. Need to send a message.
	int recv_phyid = AosGetIILPhysicalid(rdata, iilname);
	aos_assert_fr(AosIsValidPhysicalId(recv_phyid), rdata, false);
	AosCubeMsgPtr req = OmnNew AosMsgRetrvIILBlock(
			rdata, AosGetSelfPhysicalId(), recv_phyid, mQueryId, 
			mIILName, mContextBuff);
	AosCubeMsgProcPtr thisptr(this, false);
	bool rslt = AosSendCubeMsg(rdata, req, thisptr);
	aos_assert_fr(rslt, rdata, false);
	return true;
}


bool
AosIILQueryReq::sendQueryBlockResp(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		const int send_phyid)
{
	// The query block was retrieved. It is saved in mQueryBlockBuff
	AosCubeMsgPtr resp = OmnNew AosMsgRetrvQueryBlockResp(
			rdata, AosGetSelfPhysicalId(), send_phyid, mQueryId, buff);
	bool rslt = AosSendCubeMsg(rdata, resp);
	aos_assert_fr(rslt, rdata, false);
	return true;
}


bool
AosIILQueryReq::sendNodelistResp(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff,
		const int send_phyid)
{
	// The query block was retrieved. It is saved in mQueryBlockBuff
	AosCubeMsgPtr resp = OmnNew AosMsgRetrvNodelistResp(
			rdata, AosGetSelfPhysicalId(), send_phyid, mQueryId, 
			buff);
	bool rslt = AosSendCubeMsg(rdata, resp);
	aos_assert_fr(rslt, rdata, false);
	return true;
}


bool
AosIILQueryReq::verifyQueryContext()
{
	mQueryContext = AosGetQueryContext();
	if (!mQueryContext)
	{
		queryFailed("invalid_query_context", AosFileAndLine);
		return false;
	}

	bool rslt = mQueryContext->serializeFromXml(context_tag);
	if (!rslt)
	{
		queryFailed("failed_create_context", AosFileAndLine);
		return false;
	}

	return true;
}


bool
AosIILQueryReq::setSortIILNodeList(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &nodelist)
{
	// The IIL has retrieved all the nodes for the block. 
	// This function needs to retrieve all the level 0 and 1
	// bitmaps and OR them with the front and tail bitmaps.
	// 
	// 'nodelist' is in the following format:
	// 		node_id			u64
	// 		parent_id		u64
	// 		level 0			int
	// 		node_id			u64
	// 		parent_id		u64
	// 		level 0			int
	// 		...
	// 		node_id			u64
	// 		parent_id		u64
	// 		level 1			int
	// 		num_members		int
	// 		member_id		u64
	// 		member_id		u64
	// 		...
	// 		node_id			u64
	// 		parent_id		u64
	// 		level 1			int
	// 		num_members		int
	// 		member_id		u64
	// 		member_id		u64
	// 		...
	nodelist->reset();
	u64 node_id;
	AosBitmapObjPtr parent_bitmap;
	AosBitmapObjPtr bb; 
	if (mFrontBitmap)
	{
		mBitmap = mFrontBitmap;
	}

	if (mTailBitmap)
	{
		if (mBitmap)
		{
			mBitmap->orBitmap(mTailBitmap);
		}
		else
		{
			mBitmap = mTailBitmap;
		}
	}

	while ((node_id = nodelist->getU64(0)) != 0)
	{
		u64 parent_id = nodelist->getU64(0);
		parent_bitmap = getBitmap(rdata, parent_id);
		int level = nodelist->getInt(-1);
		aos_assert_rr(verifyLevel(level), rdata, false);
		if (level == 0)
		{
			// It needs to retrieve
			bb = getLeafBitmap(rdata, parent_bitmap, node_id);
			aos_assert_rr(verifyBitmap(rdata, bb), rdata, false);
			if (mBitmap)
			{
				mBitmap->orBitmap(bb);
			}
			else
			{
				mBitmap = bb;
			}
		}
		else if (level == 1)
		{
			bb = getLevelOneBitmap(rdata, node_id);
			aos_assert_rr(verifyBitmap(rdata, bb), rdata, false);
			if (!mBitmap)
			{
				mBitmap = bb;
			}
			else
			{
				mBitmap->orBitmap(bb);
			}
		}
		else
		{
			AosSetErrorU(rdata, "not_supported_yet");
			queryFailed(AosFileAndLine);
			return false;
		}
	}

	return true;
}


bool
AosIILQueryReq::procSection(
		const AosRundataPtr &rdata, 
		u64 &section_id, 
		const AosBuffPtr &bitmap_buff, 
		const AosBuffPtr &nodelist_buff, 
		const AosBuffPtr &req_buff)
{
	// This function processes one section. 
	// bitmap_buff is in the following format:
	// 		section_id				(for sorting IIL)
	// 		iilidx					(for sorting IIL)
	// 		bitmap buff				(for sorting IIL)
	// 		section_id				(for condition IIL)
	// 		iilidx					(for condition IIL)
	// 		bitmap buff				(for condition IIL)
	// 		...
	// and the nodelist buff is in the following format:
	// 		section_id				(for sorting IIL)
	// 		iilidx					(for sorting IIL)
	// 		nodelist buff			(for sorting IIL)
	// 		section_id				(for condition IIL)
	// 		iilname					(for condition IIL)
	// 		nodelist buff			(for condition IIL)
	// 		...

	// Note that the section_id has already been retrieved
	// for bitmap_buff. 
	OmnString iilname;
	AosBuffPtr buff;
	u64 secid = section_id;
	while (secid == section_id)
	{
		int iilidx = bitmap_buff->getInt(-1);
		if (iilidx < 0 || (u32)iilidx >= mIILInfo.size())
		{
			OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
			queryFailed(rdata, errmsg);
			return false;
		}
		
		buff = bitmap_buff->getBuffAsBinary();
		if (!buff)
		{
			OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
			queryFailed(rdata, errmsg);
			return false;
		}

		mIILInfo[iilidx]->setBitmap(buff);
		secid = bitmap_buff->getU64(AOS_INV_SECID);
	}

	u64 new_secid = secid;
	secid = section_id;
	while (secid == section_id)
	{
		int iilidx = nodelist_buff->getInt(-1);
		if (iilidx < 0 || (u32)iilidx >= mIILInfo.size())
		{
			OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
			queryFailed(rdata, errmsg);
			return false;
		}
		
		buff = nodelst_buff->getBuffAsBinary();
		if (!buff)
		{
			OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
			queryFailed(rdata, errmsg);
			return false;
		}

		mIILInfo[iilidx]->setNodelistBuff(buff);
		secid = nodelist_buff->getU64(AOS_INV_SECID);

		pickRetrievedNodes(rdata, section_id, iilidx, buff, req_buff);
	}

	section_id = secid;
	return true;
}


bool
AosIILQueryReq::pickRetrievedNodes(
		const AosRundataPtr &rdata, 
		const u64 &section_id,
		const int iilidx, 
		const AosBuffPtr &buff, 
		const AosBuffPtr &req_buff)
{
	// This function retrieves the nodes for 'iilidx'. 
	// 'buff' is in the format:
	// 		bitmap level
	// 		iil level
	// 		bitmap_id
	// 		...
	int level;
	while ((level = buff->getInt(-1)) >= 0)
	{
		int iil_level = buff->getInt(-1);
		if (iil_level < 0)
		{
			OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
			queryFailed(rdata, errmsg);
			return false;
		}
		 
		u64 bitmap_id = buff->getU64(0);
		if (bitmap_id == 0)
		{
			OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
			queryFailed(rdata, errmsg);
			return false;
		}

		if (!AosGetIILCachenEngine()->isNodeCached(rdata, level, iil_level, bitmap_id))
		{
			req_buff->setInt(level);
			req_buff->setInt(iil_level);
			req_buff->setInt(iilidx);
			req_buff->setU64(bitmap_id);
		}
	}

	return true;
}


bool
AosIILQueryReq::procRetrvNodesResp(
		const AosRundataPtr &rdata, 
		const AosMsgRetrvNodesRespPtr &msg)
{
	// Some of the bitmaps were retrieved. This function caches these
	// bitmaps and then checks whether all the bitmaps are retrieved.
	// If not, do nothing. Otherwise, it needs to compute the results.
	u64 section_id;
	AosBuffPtr buff = msg->getBuff();
	while ((section_id = buff->getU64(AOS_INV_SECID)) != AOS_INV_SECID)
	{
		int iilidx = buff->getInt(-1);
		int level = buff->getInt(-1);
		int iil_level = buff->getInt(-1);
		u64 bitmap_id = buff->getU64(0);
		AosBuffPtr bitmap_buff = buff->getBuffAsBinary();
		aos_assert_rr(verifyIILIdx(iilidx), rdta, false);
		aos_assert_rr(verifyLevel(level), rdta, false);
		aos_assert_rr(verifyIILLevel(iil_level), rdta, false);
		aos_assert_rr(verifyBitmapId(bitmap_id), rdta, false);
		aos_assert_rr(verifyBitmapBuff(bitmap_buff), rdta, false);

		AosGetIILCacheEngine()->cacheBitmapNode(rdata, 
				level, iil_level bitmap_id, bitmap_buff);
	}

	if (msg->isFinished()) return computeLocalResults(rdata);
	return true;
}


bool 
AosIILQueryReq::computeLocalResults(const AosRundataPtr &rdata)
{
	// All needed bitmaps have been retrieved from the remote servers. 
	// It is the time to compute the results. 
	//
	// There are one sorting IIL and one or more condition IILs. 
	// For each IIL, it retrieves all the bitmap nodes based on its
	// node list. It then ORs the bitmap with its partial bitmap. 
	// After that, it ANDs the results with the sorting IIL. 
	//
	// Note that there are multiple sections. The above needs to loop
	// over all the sections. 
	
	mSortingIIL->reset();
	u64 section_id;
	AosBitmapObjPtr final_bitmap, bitmap;
	while ((section_id = mSortingIIL->getNextSection(rdata, bitmap)) != AOS_INV_SECID)
	{
		if (!bitmap)
		{
			OmnString errmsg = getErrmsg("internal_error");
			queryFailed(rdata, errmsg);
			return false;
		}

		for (u32 i=0; i<mIILs.size(); i++)
		{
			AosBitmapObjPtr bb;
			bool rslt = mIILs[i]->getBitmap(rdata, section_id, bb);
			if (!rslt)
			{
				OmnString errmsg = getErrmsg("internal_error");
				queryFailed(rdata, errmsg);
				return false;
			}

			if (!bb)
			{
				// This means there are no results for this section 
				// for this iil, which means there are no results 
				// for this section.
				bitmap = 0;
				break;
			}

			// There are some results from this iil. AND its results
			// with 'bitmap'.
			bitmap->andBitmap(bb);
			if (bitmap->isEmpty())
			{
				// There are no results for this section.
				bitmap = 0;
				break;
			}
		}

		if (bitmap)
		{
			if (bitmap->isEmpty())
			{
				OmnString errmsg = getErrmsg("internal_error");
				queryFailed(rdata, errmsg);
				return false;
			}

			// There are some results for this section. Add them into the
			// final results.
			if (final_bitmap)
			{
				final_bitmap->orBitmap(bitmap);
			}
			else
			{
				final_bitmap = bitmap;
			}
		}
	}

	AosMsgSectionResultsPtr msg = OmnNew AosMsgSectionResults(
			rdata, smSelfPhyId, mSenderPhyId, final_bitmap);
	bool rslt = AosSendCubeMsg(rdata, msg);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosIILQueryReq::procSectionResults(
		const AosRundataPtr &rdata, 
		const AosMsgSectionResultsRespPtr &msg)
{
	// A physical machine has finished retrieving its results. 
	// This function assembles its results into its running 
	// results. It then checks whether this is the last physical
	// to respond. If yes, it will calculate the final results. 
	// Afer that, it sends the response back to the requester.
	
	int send_phyid = msg->getSendPhysicalId();
	if (!AosIsValidPhysicalId(send_phyid))
	{
		OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
		queryFailed(rdata, errmsg);
		return false;
	}

	mLock->lock();
	mIILs[send_phyid].responded = true;
	AosBuffPtr result_buff = msg->getResults();
	mNumResponded++;
	if (mNumResponded < mNumPhysicals)
	{
		// Not finished yet.
		mLock->unlock();
		addResults(rdata, result_buff);
		return true;
	}

	// All responded. It is the time to compute the final results.
	addResults(rdata, result_buff);
	bool rslt = computeFinalResults(rdata);
	mLock->unlock();
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosIILQueryReq::addResults(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	if (!buff && buff->dataLen() <= 0) return true;
	
	AosBitmapObjPtr bitmap = AosGetBitmap();
	bitmap->loadFromBuff(buff);
	if (mFinalResults)
	{
		mFinalResults->orBitmap(bitmap);
	}
	else
	{
		mFinalResults = bitmap;
	}

	return true;
}

	
bool 
AosIILQueryReq::computeFinalResults(const AosRundataPtr &rdata)
{
	// Results have been received from all physical servers. 
	// The final results are in mFinalResults. What need to do 
	// now is to go over all the sorting IIL nodes. For each 
	// sorting IIL node, it uses its bitmap to determine whether
	// there are results for it. If yes, it retrieves the docids
	// and then filter it.
	
	AosBuffPtr final_results = OmnNew AosBuff();

	// 1. Process the front IIL. It may or may not have it.
	AosBitmapObjPtr bb = getFrontBitmap(rdata);
	if (bb)
	{
		// The front IIL is not empty. Check whether there are 
		// results in the front IIL.
		bb->andBitmap(mFinalBitmap);
		if (!bb->isEmpty())
		{
			// The front IIL has some results. Need to find
			// the matching docids for it.
			AosBuffArrayPtr buffarray = getFrontDocids(rdata);
			if (!buffarray)
			{
				queryFailed(rdata, query, "internal_error", "", AosFileAndLine);
				return false;
			}

			processLeaf(rdata, results, buffarray, bb);
		}
	}

	// 2. Process the subsequent iils. There are level 0 and level 1
	//    bitmaps. These were constructed when the query block was 
	//    retrieved. It is a sequence of nodes. Some nodes are level 
	//    0 bitmaps and some level 1 bitmaps. Below is a loop that 
	//    loops over all the nodes.
	int level;
	u64 iilid;
	while ((bb = nextBitmap(rdata, iilid, level)))
	{
		// Found one node (i.e., a level 0/1 bitmap). Check whether
		// is contains results.
		bb->andBitmap(mFinalBitmap);
		if (!bb->isEmpty())
		{
			// There are some results in the bitmap. In the current
			// implementations, the node must be either level 0 or 
			// level 1. In the future, we may want to consider including
			// level 2 nodes for the query block.
			if (level != 1 || level 1= 0)
			{
				OmnString errmsg = getErrmsg("invalid_level", "level", level, AosFileAndLine);
				queryFailed(rdata, errmsg);
				return false;
			}

			// It is not empty. Check the results.
			if (level == 1)
			{
				// This is a Level 1 bitmap. It needs to loop over
				// all the level 0 bitmaps. 
				int idx = 0;
				AosBitmapObjPtr mm;
				while ((mm = query->getLeafBitmap(rdata, iilid, idx)))
				{
					mm->andBitmap(bb);
					if (!mm->isEmpty())
					{
						// There are some results in this level 0 IIL
						AosBuffPtr buffarray = 
							AosGetIILCacheEngine()->getDocids(rdata, iilid);
						if (!buffarray)
						{
							OmnString errmsg = getErrmsg("internal_error", 
									"iilid", iilid, AosFileAndLine);
							queryFailed(rdata, errmsg);
							return false;
						}
						processLeaf(rdata, results, buffarray, mm);
					}
					idx++;
				}
			}
			else
			{
				// This is a level 0 IIL. It contains some results.
				AosBuffArrayPtr buffarray = 
					AoGetIILCacheEngine()->getDocids(rdata, iilid);
				if (!buffarray)
				{
					OmnString errmsg = getErrmsg("internal_error", 
									"iilid", iilid, AosFileAndLine);
					queryFailed(rdata, errmsg);
					return false;
				}
				processLeaf(rdata, results, buffarray, bb);
			}
		}
	}

	// Need to process the last IIL.
	bb = getTailBitmap(rdata);
	if (bb)
	{
		bb->andBitmap(mFinalBitmap);
		if (!bb->isEmpty())
		{
			AosIILCacheEnginePtr thisptr(this, false);
			AosBuffArrayPtr buffarray = getTailDocids(rdata, thisptr);
			if (!buffarray)
			{
				OmnString errmsg = getErrmsg("internal_error", AosFileAndLine);
				queryFailed(rdata, errmsg);
				return false;
			}

			processLeaf(rdata, results, buffarray, bb);
		}
	}

	// It is the time to send the final results to the requester.
	AosMsgBmpQueryFinalRsltsPtr msg = OmnNew AosMsgBmpQueryFinalRslts(
			rdata, AosGetSelfPhysicalId(), mQuerySendPhyId, mQueryId, 
			mFinalResults);
	bool rslt = AosSendCubeMsg(rdata, msg);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosIILQueryReq::retrieveNodelist(
		const AosRundataPtr &rdata,
		const AosMsgRetrvNodelistPtr &msg)
{
	aos_query_assert_r(msg, rdata, false);

	OmnString iilname = msg->getIILName();
	aos_query_assert_r(iilname != "", rdata, false);
	AosBuffPtr context_buff = msg->getQueryContextBuff();
	aos_query_assert_r(context_buff, rdata, false);

	AosQueryContextObjPtr context = AosCreateQueryContext(buff);
	aos_query_assert_r(context, rdata, false);

	// Now we have collected all the information. It is the time to 
	// retrieve the block.
	AosQueryReqObjPtr query(thisptr, false);
	AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
	aos_assert_fr(iilmgr, rdata, false);

	// Check whether it is locally cached.
	AosBuffPtr result_buff;
	bool cached;
	bool rslt = AosGetIILCacheEngine()->getQueriedData(rdata, iilname, 
			context_buff, 0, result_buff, cached);
	if (cached)
	{
		// It is locally cached. 
		rslt = sendNodelistResp(rdata, result_buff);
		aos_assert_fr(rslt, rdata, false);
		return true;
	}

	AosBitmapObjPtr bitmap;
	AosBuffPtr nodelist;
	AosDataletPtr datalet = AosCreateDatalet();
	datalet->set
	bool rslt = iilmgr->retrieveNodelist(rdata, mIILName, mFinished, 
			bitmap, nodelist, query);
	aos_assert_fr(rslt, rdata, false);

	AosBuffPtr bmp_buff;
	if (bitmap) bitmap->saveToBuff(bmp_buff);
	AosMsgRetrvNodelistRespPtr resp = OmnNew AosMsgRetrvNodelistResp(
			rdata, AosGetSelfPhysicalId(), msg->getSendPhyId(), msg->getQueryId(), 
			bitmap_buff, nodelist);

	rslt = AosSendCubeMsg(rdata, resp);
	aos_assert_fr(rslt, rdata, false);
	return true;
}


bool 
AosIILQueryReq::getLocalCachedBlock(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &context_buff,
		const OmnString &iilname,
		const u64 &block_start_pos,
		AosBuffPtr &result_buff,
		bool &cached)
{
	aos_assert_fr(context_buff, rdata, false);
	aos_assert_fr(iilname != "", rdata, false);
	bool rslt = AosGetIILCacheEngine()->getQueriedData(rdata, iilname, 
			context_buff, block_start_pos, result_buff, cached);
	aos_assert_fr(rslt, rdata, false);
	return true;
}


bool
AosIILQueryReq::procRetrvIILBlockResp(
		const AosRundataPtr &rdata, 
		const AosMsgRetrvIILBlockRespPtr &msg)
{
	// The query block was retrieved from a remote server. 
	aos_assert_fr(msg, rdata, false);
	OmnString iilname = msg->getIILName();
	AosBuffPtr results_buff = msg->getBlockBuff();
	AosBuffPtr context_buff = msg->getContextBuff();
	u64 query_id = msg->getQueryId();
	u64 total_docs = msg->getTotalDocs();
	u64 picked_docs = msg->getPickedDocs();
	bool finished = msg->getFinishedFlag();

	aos_assert_fr(iilname != "", rdata, false);
	aos_assert_fr(iilname == mIILName, rdata, false);
	aos_assert_fr(query_id == mQueryId, rdata, false);
	aos_assert_fr(context_buff, rdata, false);
	
	AosDatalet dd = AosCreateDatalet(AosSmanIILCachedData());
	dd->setField(AosSmanIILCachedData::eBuff, results_buff);
	dd->setField(AosSmanIILCachedData::eQueryContext, context);
	dd->setField(AosSmanIILCachedData::eBlockStartPos, mBlockStartPos);
	aos_assert_fr(dd->isCorrect(), rdata, false);
	bool rslt = AosGetIILCacheEngine()->cacheBlock(rdata, dd);
	aos_assert_fr(rslt, rdata, false);

	rslt = sendQueryBlockResp(rdata, results_buff, msg->getSendPhyId());
	aos_assert_fr(rslt, rdata, false);
	return true;
}
#endif
