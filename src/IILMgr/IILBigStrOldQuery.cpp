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
// 	Created: 2012/04/16 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILBigStr.h"

#include "IILMgr/IILMgr.h"


bool
AosIILBigStr::queryRangePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function???????????????????
//shawn single function should be isolated
// need to verify start/end 
	aos_assert_r(query_context, false);
	AosOpr startOpr = query_context->getOpr();
	AosOpr endOpr;
	
	if (startOpr == eAosOpr_range_ge_le)
	{
	    startOpr = eAosOpr_ge;
	    endOpr = eAosOpr_le;
	}
	else if (startOpr == eAosOpr_range_ge_lt)
	{
	    startOpr = eAosOpr_ge;
	    endOpr = eAosOpr_lt;
	}
	else if (startOpr == eAosOpr_range_gt_le)
	{
	    startOpr = eAosOpr_gt;
	    endOpr = eAosOpr_le;
	}
	else if (startOpr == eAosOpr_range_gt_lt)
	{
	    startOpr = eAosOpr_gt;
	    endOpr = eAosOpr_lt;
	}
	else
	{
	    OmnAlarm << "Operation Error, not range opr:" << AosOpr_toStr(startOpr) << enderr;
	}
	
	aos_assert_r(startOpr == eAosOpr_gt || startOpr == eAosOpr_ge, false);
	aos_assert_r(endOpr == eAosOpr_lt || endOpr == eAosOpr_le, false);
	OmnString startValue = query_context->getStrValue();
	OmnString endValue = query_context->getStrValue2();

	bool rslt = false;
	u64 docid;
	bool isunique = false;

	AosIILIdx start_idx;
	AosIILIdx end_idx;
	if (isSingleIIL())
	{
		rslt = nextDocidPrivFind(start_idx, startValue, startOpr, docid, isunique, false, rdata);
		aos_assert_r(rslt, false);
	
		rslt = nextDocidPrivFind(end_idx, endValue, endOpr, docid, isunique, true, rdata);
		aos_assert_r(rslt, false);

		// copy data
	//	rslt = copyDocidsPriv(query_rslt, query_bitmap, idx_start, idx_end);
	//	aos_assert_r(rslt, false);

//error need fix.
		return false;
	}

	rslt = nextDocidPrivFind(start_idx, startValue, startOpr, docid, isunique, false, rdata);
	if (!rslt)
	{
		// This means that it did not find anything.
		return true;
	}
	
	rslt = nextDocidPrivFind(end_idx, endValue, endOpr, docid, isunique, true, rdata);
	if (!rslt)
	{
		OmnAlarm << "Internal error: " << rslt << ":" << end_idx.toString() << enderr;
		return false;
	}
	
//error need fix.
	return false;
	/*
	// count num of docs
	int num_docs = 0;
	for(int i=iilidx_start; i<iilidx_end; i++)
	{
		num_docs += mNumEntries[i];
	}
	num_docs += idx_end + 1;
	num_docs -= idx_start;

	// copy data
	AosIILBigStrPtr subiil;
	int startpos = -10;
	int endpos = -10;
	for(int i=iilidx_start; i<=iilidx_end; i++)
	{
		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(), false);

		if (i == iilidx_start)
		{
			startpos = idx_start;
		}
		else
		{
			startpos = 0;
		}

		if (i == iilidx_end)
		{
			endpos = idx_end;
		}
		else
		{
			endpos = -10;
		}

		if (i == 0)
		{
			rslt = subiil->copyDocidsPriv(query_rslt, query_bitmap, startpos, endpos);
			aos_assert_r(rslt, false);
		}
		else
		{
			rslt = subiil->copyDocidsSafe(query_rslt, query_bitmap, startpos, endpos);
			aos_assert_r(rslt, false);
		}
	}
	return true;
	*/
}


//////////////////////////////////////////////////////////////////////////
//			Below are root level member functions						//
//////////////////////////////////////////////////////////////////////////

bool
AosIILBigStr::queryPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}


/*
bool
AosIILBigStr::queryNewNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	// shawn query function
	// It is to query the Not-Equal. There shall be two segments.
	// 1. We get 2 pieces of data which contain the data we need.
	bool block_exist_1 = true;
	bool block_exist_2 = true;
	int block_start_idx_1 = 0;
	int block_start_iilidx_1 = 0;
	int block_end_idx_1 = mNumDocs -1;
	int block_end_iilidx_1 = 0;

	int block_start_idx_2 = 0;
	int block_start_iilidx_2 = 0;
	int block_end_idx_2 = mNumDocs -1;
	int block_end_iilidx_2 = 0;
	u64 docid = 0;
	if (isRootIIL())
	{
		block_end_idx_1 = mNumEntries[mNumSubiils -1]-1;
		block_end_iilidx_1 = mNumSubiils -1;
		block_end_idx_2 = mNumEntries[mNumSubiils -1]-1;
		block_end_iilidx_2 = mNumSubiils -1;
	}	
	
	OmnString value = query_rslt->getStrValue();

	// Find the first subiil that satisfies the condition
	int cur_idx = -10;
	int cur_iilidx = -10;
	bool is_unique = false;
	bool rslt = nextDocidPriv(cur_idx, cur_iilidx, false, 
			eAosOpr_eq, value, docid, is_unique, rdata);
	aos_assert_r(rslt,false);
	if (cur_idx >= 0)
	{
		//1. set block 1 infomation
		if (cur_idx == 0)
		{
			if (cur_iilidx == 0)
			{
				block_exist_1 = false;
			}else
			{
				block_end_iilidx_1 = cur_iilidx -1;
				block_end_idx_1 = mNumEntries[block_end_iilidx_1]-1;
			}
		}
		else
		{
			block_end_idx_1 = cur_idx -1;
			block_end_iilidx_1 = cur_iilidx;
		}
		
		//2. set block 2 infomation
		cur_idx = -10;
		cur_iilidx = -10;
		rslt = nextDocidPriv(cur_idx, cur_iilidx, true,eAosOpr_eq, value, docid, is_unique, rdata);
		aos_assert_r(rslt && cur_idx >= 0,false);
		if (isSingleIIL())
		{
			if (cur_idx >= mNumDocs -1)
			{
				block_exist_2 = false;				
			}
			else
			{
				block_start_idx_2 = cur_idx +1;
				block_start_iilidx_2 = 0;
			}
		}
		else// root iil
		{
			if (cur_idx >= mNumEntries[cur_iilidx] -1)
			{
				block_start_idx_2 = 0;
				block_start_iilidx_2 = cur_iilidx +1;
				if (block_start_iilidx_2 >= mNumSubiils)
				{
					block_exist_2 = false;								
				}
			}
			else
			{
				block_start_idx_2 = cur_idx +1;
				block_start_iilidx_2 = cur_iilidx;
			}
		}
	}
	else
	{
		block_exist_2 = false;
	}
	
	// 2. Get the current index from query rslt and fix it if it's wrong
	cur_idx = query_rslt->getIndex();
	cur_iilidx	= query_rslt->getIILIndex();
	OmnString cur_value = query_rslt->getCrtValue();
	u64 cur_docid = query_rslt->getCrtDocid();
	bool has_data = true;
	bool reverse = query_rslt->isReverse();
		
	if (cur_idx == -10 || cur_iilidx == -10)
	{
		//no need to change current position
	}
	else
	{
		aos_assert_r(cur_idx >= 0 && cur_iilidx >= 0,false);
		nextIndexFixErrorIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		if (!has_data)
		{
			query_rslt->setFinished(true);
			return true;
		}
	}

	// Now we count the real start/end we need in the 2 pieces of data		
	int start_pos = 0;
	int end_pos = 0;
	bool finished = true;	

	int doc_needed = query_rslt->getBlockSize();
	if (reverse)
	{
		// start from block 2
		if (block_exist_2)
		{
			if (compPos(cur_idx,cur_iilidx,block_end_idx_2,block_end_iilidx_2) < 0)
			{
				block_end_idx_2		= cur_idx;
				block_end_iilidx_2	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_start_idx_2,block_start_iilidx_2) < 0)
			{
				block_exist_2 = false;
			}
			else
			{
				for(int i = block_end_iilidx_2;i >= block_start_iilidx_2;i--)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_2)start_pos = block_start_idx_2;
					if (i == block_end_iilidx_2)end_pos = block_end_idx_2;

					if (end_pos - start_pos +1 > doc_needed)
					{
						// we found the end
						block_exist_1 = false;
						block_start_iilidx_2 = i;
						block_start_idx_2 = end_pos - doc_needed +1;
						finished = false;	
						query_rslt->setIndex(block_start_idx_2);
						query_rslt->setIILIndex(block_start_iilidx_2);
						query_rslt->setCrtDocid(getIILByIndex(block_start_iilidx_2,true,rdata)->getDocid(block_start_idx_2));
						query_rslt->setCrtValue(getIILByIndex(block_start_iilidx_2,true,rdata)->getValue(block_start_idx_2));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
		if (!doc_needed && block_exist_1)
		{
			finished = false;
		}
		
		if (doc_needed && block_exist_1)
		{
			if (compPos(cur_idx,cur_iilidx,block_end_idx_1,block_end_iilidx_1) < 0)
			{
				block_end_idx_1		= cur_idx;
				block_end_iilidx_1	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_start_idx_1,block_start_iilidx_1) < 0)
			{
				block_exist_1 = false;
			}
			else
			{
				for(int i = block_end_iilidx_1;i >= block_start_iilidx_1;i--)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_1)start_pos = block_start_idx_1;
					if (i == block_end_iilidx_1)end_pos = block_end_idx_1;

					if (end_pos - start_pos +1 >= doc_needed)
					{
						// we found the end
						block_start_iilidx_1 = i;
						block_start_idx_1 = end_pos - doc_needed +1;
						finished = false;
						query_rslt->setIndex(block_start_idx_1);
						query_rslt->setIILIndex(block_start_iilidx_1);
						query_rslt->setCrtDocid(getIILByIndex(block_start_iilidx_1,true,rdata)->getDocid(block_start_idx_1));
						query_rslt->setCrtValue(getIILByIndex(block_start_iilidx_1,true,rdata)->getValue(block_start_idx_1));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
	}
	else // normal order
	{
		// start from block 1
		if (block_exist_1)
		{
			if (compPos(cur_idx,cur_iilidx,block_start_idx_1,block_start_iilidx_1) > 0)
			{
				block_start_idx_1		= cur_idx;
				block_start_iilidx_1	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_end_idx_1,block_end_iilidx_1) > 0)
			{
				block_exist_1 = false;
			}
			else
			{
				for(int i = block_start_iilidx_1;i <= block_end_iilidx_1;i++)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_1)start_pos = block_start_idx_1;
					if (i == block_end_iilidx_1)end_pos = block_end_idx_1;

					if (end_pos - start_pos +1 >= doc_needed)
					{
						// we found the end
						block_exist_2 = false;
						block_end_iilidx_1 = i;
						block_end_idx_1 = start_pos + doc_needed -1;
						finished = false;	
						query_rslt->setIndex(block_end_idx_1);
						query_rslt->setIILIndex(block_end_iilidx_1);
						query_rslt->setCrtDocid(getIILByIndex(block_end_iilidx_1,true,rdata)->getDocid(block_end_idx_1));
						query_rslt->setCrtValue(getIILByIndex(block_end_iilidx_1,true,rdata)->getValue(block_end_idx_1));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
		if (!doc_needed && block_exist_2)
		{
			finished = false;
		}
		
		if (doc_needed && block_exist_2)
		{
			if (compPos(cur_idx,cur_iilidx,block_start_idx_2,block_start_iilidx_2) > 0)
			{
				block_start_idx_2		= cur_idx;
				block_start_iilidx_2	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_end_idx_2,block_end_iilidx_2) > 0)
			{
				block_exist_2 = false;
			}
			else
			{
				for(int i = block_end_iilidx_2;i <= block_start_iilidx_2;i++)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_2)start_pos = block_start_idx_2;
					if (i == block_end_iilidx_2)end_pos = block_end_idx_2;

					if (end_pos - start_pos +1 >= doc_needed)
					{
						// we found the end
						block_start_iilidx_2 = i;
						block_start_idx_2 = start_pos + doc_needed -1;
						finished = false;
						query_rslt->setIndex(block_end_idx_2);
						query_rslt->setIILIndex(block_end_iilidx_2);
						query_rslt->setCrtDocid(getIILByIndex(block_end_iilidx_2,true,rdata)->getDocid(block_end_idx_2));
						query_rslt->setCrtValue(getIILByIndex(block_end_iilidx_2,true,rdata)->getValue(block_end_idx_2));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
	}
	
	// copy data
	query_rslt->setFinished(finished);
	if (reverse)
	{
		query_rslt->setDataReverse(true);
	}
	if (block_exist_1)
	{
		copyData(query_rslt,
				 query_bitmap,
				 block_start_idx_1,
				 block_start_iilidx_1,
				 block_end_idx_1,
				 block_end_iilidx_1,
				 rdata);
	}
	
	if (block_exist_2)
	{
		copyData(query_rslt,
				 query_bitmap,
				 block_start_idx_2,
				 block_start_iilidx_2,
				 block_end_idx_2,
				 block_end_iilidx_2,
				 rdata);
	}

	return true;
}


bool
AosIILBigStr::queryNewLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	// shawn query function
	int cur_idx = query_rslt->getIndex();
	bool reverse = query_rslt->isReverse();
	int cur_iilidx = query_rslt->getIILIndex();

	if (cur_idx >= 0)
	{
		bool has_data = true;
		OmnString cur_value = query_rslt->getCrtValue();
		u64 cur_docid = query_rslt->getCrtDocid();
		aos_assert_r(cur_iilidx >= 0,false);
		nextIndexFixErrorIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
	}
	else
	{
		// cur_idx < 0
		if (reverse)
		{
			if (isSingleIIL())
			{
				cur_idx = mNumDocs -1;
				cur_iilidx = 0;
			}
			else
			{
				cur_idx = mNumDocs -1;
				cur_iilidx = mNumSubiils-1;
			}
		}
		else
		{
			cur_idx = 0;
			cur_iilidx = 0;
		}
	}

	if (isSingleIIL())
	{
		queryNewLikeSinglePriv(query_rslt,query_bitmap,cur_idx,reverse,rdata);
		if (!query_rslt->finished())
		{
			aos_assert_r(cur_idx >= 0 && cur_idx <= mNumDocs -1,false);
			query_rslt->setIndex(cur_idx);
			query_rslt->setIILIndex(0);
			query_rslt->setCrtDocid(mDocids[cur_idx]);
			query_rslt->setCrtValue(mValues[cur_idx]);
		}
		return true;
	}
	
	if (reverse)
	{
		while(cur_iilidx >= 0)
		{
			getIILByIndex(cur_iilidx,true,rdata)->queryNewLikeSinglePriv(
					query_rslt,query_bitmap,cur_idx,reverse,rdata);
			if (cur_iilidx != 0)
			{
				query_rslt->setFinished(false);
			}
			if (query_rslt->isFull())
			{
				break;
			}
			cur_iilidx --;
			if (cur_iilidx >= 0)cur_idx = mNumEntries[cur_iilidx]-1;
		}
	}
	else
	{
		while(cur_iilidx < mNumSubiils)
		{
			getIILByIndex(cur_iilidx,true,rdata)->queryNewLikeSinglePriv(
					query_rslt,query_bitmap,cur_idx,reverse,rdata);
			if (cur_iilidx != mNumSubiils -1)
			{
				query_rslt->setFinished(false);
			}
			if (query_rslt->isFull())
			{
				break;
			}
			cur_iilidx ++;
			cur_idx = 0;
		}
	}

	if (!query_rslt->finished())
	{
		aos_assert_r(cur_iilidx >= 0 && cur_iilidx <= mNumSubiils -1,false);
		aos_assert_r(cur_idx >= 0 && cur_idx <= mNumEntries[cur_iilidx] -1,false);
		query_rslt->setIndex(cur_idx);
		query_rslt->setIILIndex(cur_iilidx);
		query_rslt->setCrtDocid(getIILByIndex(cur_iilidx,true,rdata)->getDocid(cur_idx));
		query_rslt->setCrtValue(getIILByIndex(cur_iilidx,true,rdata)->getValue(cur_idx));
	}
	return true;
}


bool
AosIILBigStr::queryNewLikeSinglePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const int &start_idx,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	// shawn query function
	int cur_idx = start_idx;
	bool search_over = false;
	OmnString value = query_rslt->getStrValue();
	while(!search_over)
	{
		if (valueLikeMatch(mValues[cur_idx],value))
		{
			// append docid
			query_rslt->appendDocid(mDocids[cur_idx]);
			if (query_rslt->isFull())
			{
				search_over = true;
				break;
			}
		}
		//check if it is the last one
		if (reverse)
		{
			if (cur_idx == 0)
			{
				query_rslt->setFinished(true);
				search_over = true;
			}
			else
			{
				cur_idx --;
			}
		}
		else// normal order
		{
			if (cur_idx == mNumDocs-1)
			{
				query_rslt->setFinished(true);
				search_over = true;
			}
			else
			{
				cur_idx ++;
			}
		}
		if (query_rslt->isFull())
		{
			search_over = true;
		}
	}
	return false;
}


bool
AosIILBigStr::queryPagingProc(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		int &start_idx,
		int &start_iilidx,
		int &end_idx,
		int &end_iilidx,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	// This function is used for query. Its condition is defined by 'query_rslt'.
	// Its mission is to find the starting and ending positions that satisfies
	// the condition defined in 'query_rslt'. It then checks the starting position
	// and block size to make sure:
	// 		[start_idx/start_iilidx, end_idx/end_iilidx]
	// is the range that satisfies the condition and the value range that needs
	// to be copied.
	//	1. get the start idx/iilidx
	//	2. get the end idx/iilidx
	//	3. get the cur idx/iilidx
	//	4. get the block start/end  idx/iilidx
	//
	// Chen Ding, 04/18/2012
	// 'query_bitmap' is not used!!!!!!!!!!!!!!!!!!!!!!!
           
	bool rslt = false;
	bool reverse = false;	
	has_data = true;
	if (query_rslt && query_rslt->finished())
	{
		has_data = false;
		return true;
	}

	reverse = query_rslt->isReverse();
	AosOpr opr = query_rslt->getOpr();
	OmnString str_value = query_rslt->getStrValue();

	//1. get the start idx/iilidx
	start_idx = -10;
	start_iilidx = -10;
	u64 cur_docid_0 = 0;
	bool isunique = false;
	rslt = nextDocidPriv(start_idx, 
						 start_iilidx,
						 false, // normal order 
						 opr,
						 str_value,
						 cur_docid_0, 
						 isunique,
						 rdata);
	aos_assert_r(rslt,false);
	if (start_idx < 0)
	{
		has_data = false;
		return true;
	}
	
	//2. get the end idx/iilidx	
	end_idx = -10;
	end_iilidx = -10;
						 
	rslt = nextDocidPriv(end_idx, 
						 end_iilidx,
						 true, // normal order 
						 opr,
						 str_value,
						 cur_docid_0, 
						 isunique,
						 rdata);
	aos_assert_r(rslt,false);
	aos_assert_r(end_idx >= 0,false);
	
	if (isSingleIIL())
	{
		start_iilidx = 0;
		end_iilidx = 0;
	}
	
	//3. get the cur idx/iilidx, to fix start_idx/iilidx & end_idx/iilidx
	int cur_idx = query_rslt->getIndex();
	int cur_iilidx = query_rslt->getIILIndex();
	
	if (cur_idx == -10 || cur_iilidx == -10)
	{
		// no need to change current position
	}
	else
	{
		aos_assert_r(cur_idx >= 0 && cur_iilidx >= 0,false);

		OmnString cur_value = query_rslt->getCrtValue();
		u64 cur_docid = query_rslt->getCrtDocid();
		nextIndexFixErrorIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		if (!has_data)
		{
			return true;
		}
		
		if (!reverse)
		{
			start_idx = cur_idx;
			start_iilidx = cur_iilidx;
		}
		else
		{
			end_idx = cur_idx;
			end_iilidx = cur_iilidx;
		}
	}
	
	// here is a check, if there are no more docs there, set has_data to false and return.
	if (end_iilidx < start_iilidx || ((end_iilidx == start_iilidx) &&(end_idx < start_idx)))
	{
		has_data = false;
		return true;
	}
	
	//4. get the block start/end  idx/iilidx
	int new_cur_idx = 0;
	int new_cur_iilidx = 0;
	int bsize = query_rslt->getBlockSize();
	if (reverse)
	{
		if (isSingleIIL())
		{
			new_cur_idx = end_idx - bsize + 1;

			if (new_cur_idx <= start_idx)
			{
				query_rslt->setFinished(true);
				query_rslt->setIndex(0);
				query_rslt->setIILIndex(0);
				return true;
			}
			
			query_rslt->setIndex(new_cur_idx);
			query_rslt->setIILIndex(0);
			query_rslt->setCrtDocid(mDocids[new_cur_idx]);
			query_rslt->setCrtValue(mValues[new_cur_idx]);
			start_idx = new_cur_idx;
			return true;
		}
		
		// root iil
		new_cur_idx = end_idx;
		new_cur_iilidx = end_iilidx;
		int start_1 = 0;
		int end_1 = 0;
		while (bsize > 0 && new_cur_iilidx >= start_iilidx)
		{
			start_1 = (new_cur_iilidx == start_iilidx)?start_idx:0;
			end_1 = (new_cur_iilidx == end_iilidx)?end_idx:(mNumEntries[new_cur_iilidx]-1);
			
			if (end_1-start_1+1 >= bsize)
			{
				// enough doc to get
				new_cur_idx = end_1 - bsize +1;
				break;
			}
			
			// not enough to get
			if (new_cur_iilidx == start_iilidx)
			{
				// no more iils left
				new_cur_idx = start_idx;
				break;
			}
			
			new_cur_iilidx --;
			new_cur_idx = mNumEntries[new_cur_iilidx];
		}
		
		if (new_cur_iilidx == start_iilidx && new_cur_idx == start_idx)
		{
			query_rslt->setFinished(true);
			query_rslt->setIndex(0);
			query_rslt->setIILIndex(0);
		}
		else
		{
			query_rslt->setIndex(new_cur_idx);
			query_rslt->setIILIndex(new_cur_iilidx);
			query_rslt->setCrtDocid(getIILByIndex(new_cur_iilidx,true,rdata)->getDocid(new_cur_idx));
			query_rslt->setCrtValue(getIILByIndex(new_cur_iilidx,true,rdata)->getValue(new_cur_idx));
		}
		
		start_idx = new_cur_idx;
		start_iilidx = new_cur_iilidx;
		return true;
	}
	else// Normal order, not reverse
	{
		if (isSingleIIL())
		{
			new_cur_idx = start_idx + bsize - 1;

			if (new_cur_idx >= start_idx)
			{
				query_rslt->setFinished(true);
				query_rslt->setIndex(0);
				query_rslt->setIILIndex(0);
				return true;
			}
			
			query_rslt->setIndex(new_cur_idx);
			query_rslt->setIILIndex(0);
			query_rslt->setCrtDocid(mDocids[new_cur_idx]);
			query_rslt->setCrtValue(mValues[new_cur_idx]);
			end_idx = new_cur_idx;
			return true;
		}
		
		// root iil
		new_cur_idx = start_idx;
		new_cur_iilidx = start_iilidx;
		int start_1 = 0;
		int end_1 = 0;
		while(bsize > 0 && new_cur_iilidx <= end_iilidx)
		{
			start_1 = (new_cur_iilidx == start_iilidx)?start_idx:0;
			end_1 = (new_cur_iilidx == end_iilidx)?end_idx:(mNumEntries[new_cur_iilidx]-1);
			
			if (end_1-start_1+1 >= bsize)
			{
				// enough doc to get
				new_cur_idx = start_1 + bsize -1;
				break;
			}
			
			// not enough to get
			if (new_cur_iilidx == end_iilidx)
			{
				// no more iils left
				new_cur_idx = end_idx;
				break;
			}
			bsize -= (end_1-start_1+1);
			new_cur_iilidx ++;
			new_cur_idx = 0;
		}
		
		if (new_cur_iilidx == end_iilidx && new_cur_idx == end_idx)
		{
			query_rslt->setFinished(true);
			query_rslt->setIndex(0);
			query_rslt->setIILIndex(0);
		}
		else
		{
			query_rslt->setIndex(new_cur_idx);
			query_rslt->setIILIndex(new_cur_iilidx);
			query_rslt->setCrtDocid(getIILByIndex(new_cur_iilidx,true,rdata)->getDocid(new_cur_idx));
			query_rslt->setCrtValue(getIILByIndex(new_cur_iilidx,true,rdata)->getValue(new_cur_idx));
		}
		
		end_idx = new_cur_idx;
		end_iilidx = new_cur_iilidx;
		return true;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////
//				Old Query Functions
//////////////////////////////////////////////////////////////////////
bool
AosIILBigStr::queryNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function?????????????
	bool rslt = false;
	int idx_start = -10;
	int iilidx_start = -10;
	int idx_end = -10;
	int iilidx_end = -10;
	u64 docid;
	bool isunique = false;
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	
	if (idx_start >= 0)
	{	
		rslt = nextDocidPriv(idx_end, iilidx_end, true, eAosOpr_eq, value, docid, isunique, rdata);
		aos_assert_r(rslt,false);
		aos_assert_r(idx_end >= 0 && iilidx_end >= 0,false);
	}

	// count num of docs
	int num_docs = 0;

	aos_assert_r(mNumSubiils > 0,false);
	if (idx_start < 0)
	{
		// Copy all the list out
		for(int i = 0;i < mNumSubiils -1;i++)
		{
			num_docs += mNumEntries[i];
		}
	}
	else // idx_start >= 0
	{
		aos_assert_r(idx_end >= 0,false);
		for(int i = 0;i < iilidx_start;i++)
		{
			num_docs += mNumEntries[i];
		}
		num_docs += idx_start;
		num_docs += mNumEntries[iilidx_end] - idx_end - 1;
		for(u32 i = iilidx_end +1; i < mNumSubiils; i++)
		{
			num_docs += mNumEntries[i];	
		}		
	}
	
	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}

	// copy data
	int start = -10;
	int end = -10;
	AosIILBigStrPtr subiil;
	if (idx_start < 0)
	{
		// copy all data
		for(u32 i = 0;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, false, rdata);
			aos_assert_r(subiil.notNull(),false);
			if (i == 0)
			{
				rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
			else
			{
				rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
		}
		
	}
	else// there's some data, which's value == value
	{
		int start = -10;
		int end = -10;
		for(int i = 0;i <= iilidx_start;i++)
		{
			subiil = getSubiilByIndex(i, false, rdata);
			aos_assert_r(subiil.notNull(),false);
			start = 0;
			end = (i == iilidx_start)?idx_start - 1:-10;

			if (i == iilidx_start && end < start)continue;
				
			if (i == 0)
			{
				rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
			else
			{
				rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
		}			
	
		for (u32 i = iilidx_end;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, false, rdata);
			aos_assert_r(subiil.notNull(),false);


			start = (i == iilidx_end)?idx_end+1:-10;
			end = -10;

			if (i == iilidx_end && start > mNumEntries[iilidx_end] - 1)continue;

			if (i == 0)
			{
				rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
			else
			{
				rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
		}			
	}
	
	// if (query_rslt.notNull())
	// {
	// 		query_rslt->setOrdered(false);
	// }
	return true;
}


bool
AosIILBigStr::copyData(
        char** values,
        u64 *docids,
        const int buff_len,
        int &num_docs, // num_docs is in/out param
		const AosRundataPtr &rdata)
{
// shawn not right, try to remove it
	bool rslt = false;
	if (!isRootIIL())
	{
		aos_assert_r(isSingleIIL(),false);
		return copyDataSingle(values,docids,buff_len,num_docs);
	}

	// rootiil
	int doc_limit = buff_len;
	int doc_copied;
    u32 idx = 0;
    for (u32 ss=0; ss<mNumSubiils; ss++)
    {
        AosIILBigStrPtr subiil = getSubiilByIndex(ss, false, rdata);
//        aos_assert_r(idx + subiil->mNumDocs < total, false);

//		doc_copied = doc_limit;
        rslt = subiil->copyDataSingle(&(values[idx]),&(docids[idx]),doc_limit,doc_copied);
		aos_assert_r(rslt, false);
		idx += doc_copied;
		doc_limit -= doc_copied;
	
//	
//	for (u32 mm=0; mm<subiil->mNumDocs; mm++)
//        {
  //          strcpy(values[idx], mValues[mm]);
    //        docids[idx] = mDocids[mm];
      //      idx++;
//        }
//
    }
    num_docs = idx;
    return true;
}


bool
AosIILBigStr::copyDataSingle(
        char** values,
        u64 *docids,
        const int buff_len,
        int &num_docs)
{
    u32 idx = 0;
    for (u32 mm=0; mm<mNumDocs; mm++)
    {
		aos_assert_r(idx < buff_len, false);
        strcpy(values[idx], mValues[mm]);
        docids[idx] = mDocids[mm];
        idx++;
    }
    num_docs = idx;
    return true;
}


bool
AosIILBigStr::querySinglePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function?????????????
	if (opr == eAosOpr_ne)
	{
		return querySingleNEPriv(query_rslt,query_bitmap,value, rdata);
	}
	else if (opr == eAosOpr_like)
	{
		return querySingleLikePriv(query_rslt,query_bitmap,value,true, rdata);
	}
	
	bool rslt = false;
	int idx_start = -10;
	int iilidx_start = -10;
	int idx_end = -10;
	int iilidx_end = -10;
	u64 docid;
	bool isunique = false;
	
	if (opr == eAosOpr_an)
	{
		idx_start = 0;
	}
	else
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		if (idx_start < 0)
		{
			return true;
		}
	}
	
	if (opr == eAosOpr_an)
	{
		idx_end = mNumDocs - 1;
	}
	else
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt,false);
	}


	// count num of docs
	if (query_rslt.notNull())
	{
		// int num_docs = idx_end - idx_start + 1;
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	// copy data
	rslt = copyDocidsPriv(query_rslt,query_bitmap,idx_start,idx_end);
	aos_assert_r(rslt,false);

//	if (query_rslt.notNull())
//	{
//		if (opr == eAosOpr_eq)
//		{
//			query_rslt->setOrdered(true);
//		}
//		else
//		{
//			query_rslt->setOrdered(false);
//		}
//	}
	return true;
}


bool
AosIILBigStr::queryLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function?????????????
	aos_assert_r(isRootIIL(),false);
	aos_assert_r(mNumSubiils > 0, false);
	bool rslt = false;
	
	int num_docs = 0;
	for(int i = 0;i < mNumSubiils -1;i++)
	{
		num_docs += mNumEntries[i];
	}
	
	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}

	// copy data
	AosIILBigStrPtr subiil;
	for(u32 i = 0;i < mNumSubiils;i++)
	{
		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(),false);
		if (i == 0)
		{
			rslt = subiil->querySingleLikePriv(query_rslt,query_bitmap,value,false, rdata);
			aos_assert_r(rslt,false);
		}
		else
		{
			rslt = subiil->querySingleLikeSafe(query_rslt,query_bitmap,value,false, rdata);
			aos_assert_r(rslt,false);
		}
	}
	return true;
}


bool
AosIILBigStr::querySingleNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function?????????????
	bool rslt = false;
	int idx_start = -10;
	int iilidx_start = -10;
	int idx_end = -10;
	int iilidx_end = -10;
	u64 docid;
	bool isunique = false;

	if (mNumDocs <= 0)return true;
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	if (idx_start >= 0)
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, eAosOpr_eq, value, docid, isunique, rdata);
		aos_assert_r(rslt,false);
	}
	
	// count num of docs
	int num_docs = 0;
	if (idx_start < 0)
	{
		num_docs = mNumDocs;
	}
	else
	{
		num_docs = mNumDocs - (idx_end - idx_start + 1);
	}
	
	if (query_rslt)
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	// copy data
	if (idx_start < 0)
	{
		rslt = copyDocidsPriv(query_rslt,query_bitmap,0,mNumDocs -1);
		aos_assert_r(rslt,false);
		num_docs = mNumDocs;
	}
	else
	{
		// first part
		if (idx_start > 0)
		{
			rslt = copyDocidsPriv(query_rslt,query_bitmap,0,idx_start - 1);
			aos_assert_r(rslt,false);
			
		}
		// second part
		if (idx_end < mNumDocs -1)
		{
			rslt = copyDocidsPriv(query_rslt,query_bitmap,idx_end + 1, mNumDocs - 1);
			aos_assert_r(rslt,false);
		}
	}
	
	// if (query_rslt.notNull())
	// {
	// 		query_rslt->setOrdered(false);
	// }
	return true;
}


bool
AosIILBigStr::querySingleLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const bool setBlockSize,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function???????????????????
	bool rslt = false;

	if (mNumDocs <= 0)return true;
	
	if (setBlockSize && query_rslt)
	{
		// int num_docs = mNumDocs;
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	for(u32 i = 0;i < mNumDocs ;i++)
	{
	 	if (valueLikeMatch(mValues[i], value))
		{
			rslt = copyDocidsPriv(query_rslt,query_bitmap,i,i);
			aos_assert_r(rslt,false);
		}
	}
	return true;
}


bool
AosIILBigStr::queryPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function???????????????????
	if (isSingleIIL())
	{
		return querySinglePriv(query_rslt,query_bitmap,opr,value, rdata);
	}

	if (opr == eAosOpr_ne)
	{
		return queryNEPriv(query_rslt,query_bitmap,value, rdata);
	}
	else if (opr == eAosOpr_like)
	{
		return queryLikePriv(query_rslt,query_bitmap,value, rdata);
	}
	
	bool rslt = false;
	int idx_start = -10;
	int iilidx_start = -10;
	int idx_end = -10;
	int iilidx_end = -10;
	u64 docid;
	bool isunique = false;
	
	if (opr == eAosOpr_an)
	{
		idx_start = 0;
		iilidx_start = 0;
	}
	else
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value, docid, isunique, rdata);
		if (!rslt || idx_start < 0 || iilidx_start < 0)
		{
			// This means that it did not find anything.
			if (query_rslt.notNull())
			{
				// Chen Ding, 08/02/2011
				// query_rslt->setBlockSize(0);
			}
			return true;
		}
	}
	
	aos_assert_r(iilidx_start >= 0, false);
	if (opr == eAosOpr_an)
	{
		iilidx_end = mNumSubiils -1;
		idx_end = mNumEntries[iilidx_end] -1;
	}
	else
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value, docid, isunique, rdata);
		if (!rslt || idx_end < 0 || iilidx_end < 0)
		{
			OmnAlarm << "Internal error: " << rslt << ":"
				 << idx_end << ":" << iilidx_end << enderr;
			if (query_rslt.notNull())
			{
				// Chen Ding, 08/02/2011
				// query_rslt->setBlockSize(0);
			}
			return false;
		}
	}

	// count num of docs
	int num_docs = 0;
	for(int i = iilidx_start;i < iilidx_end;i++)
	{
		num_docs += mNumEntries[i];
	}
	num_docs += idx_end + 1;
	num_docs -= idx_start;

	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}

	// copy data
	AosIILBigStrPtr subiil;
	int start = -10;
	int end = -10;
	for(int i = iilidx_start;i <= iilidx_end;i++)
	{
		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(),false);
		if (i == iilidx_start)
		{
			start = idx_start;
		}
		else
		{
			start = 0;
		}

		if (i == iilidx_end)
		{
			end = idx_end;
		}
		else
		{
			end = -10;
		}

		if (i == 0)
		{
			rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
			aos_assert_r(rslt,false);
		}
		else
		{
			rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
			aos_assert_r(rslt,false);
		}
	}
	return true;
}





bool
AosIILBigStr::copyDocidsPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const int start,
		const int end)
{
// shawn query functions, 
	// Chen Ding, 12/15/2010
	aos_assert_r(mDocids, false);
	aos_assert_r(end < 0 || (u32)end < mNumDocs, false);

	int s = start;
	if (s<0)s = 0;
	int e = end;
	if (e<0)e = mNumDocs -1;
	int size = e - s + 1;
	aos_assert_r(size>=0,false);
	
	if (!query_rslt && query_bitmap)
	{
		query_bitmap->appendBlock(&(mDocids[s]),size);
		return true;
	}
	
	bool rslt = true;	
	u64 docid = 0;
	if (query_rslt)
	{
		bool addValue = query_rslt->isWithValues();
		for(int i = s;i <= e;i++)
		{
			docid = mDocids[i];
			// filter by bitmap
			if (query_bitmap && (!query_bitmap->checkDoc(docid))) continue;
			query_rslt->appendDocid(docid);
			if (addValue)
			{
				query_rslt->appendStrValue(mValues[i]);
			}
		}
	}

	return rslt;
}


int
AosIILBigStr::compPos(
	const int &idx1,
	const int &iilidx1,
	const int &idx2,
	const int &iilidx2)
{
// shawn rewrite
	if (iilidx1 != iilidx2)
	{
		return iilidx1 - iilidx2;
	}
	return idx1-idx2;
}
	
//bool
//AosIILBigStr::copyDataBlockForNE(
//		const int &start_idx,
//		const int &start_iilidx,
//		const int &end_idx,
//		const int &end_iilidx,
//		const int &cur_idx,
//		const int &cur_iilidx,
//		const AosQueryRsltObjPtr &query_rslt,
//		const AosRundataPtr &rdata)
//{
//	bool reverse = query_rslt->isReverse();
//	
//	
//}



void
AosIILBigStr::nextIndexFixErrorIdx(
		int &cur_idx,
		int &cur_iilidx,
		const OmnString &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	aos_assert(cur_idx >= 0 && cur_iilidx >=0);
	has_data = true;
	// single IIL
	if (isSingleIIL()) return nextIndexFixErrorIdxSingle(cur_idx, cur_iilidx, 
			cur_value, cur_docid, reverse, has_data, rdata);

	// It is not a leaf IIL. 
	if (cur_iilidx > mNumSubiils -1 ||
	   ((cur_iilidx == (mNumSubiils -1 )) && (cur_idx > (mNumEntries[mNumSubiils -1]-1))))
	{
		// [cur_idx, cur_iilidx] is out of the bound.
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}

	if (cur_idx > (mNumEntries[cur_iilidx]-1))
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}
	
	AosIILBigStrPtr subiil = getIILByIndex(cur_iilidx,true,rdata);
	if (!subiil)
	{
		// should not happen
		has_data = false;	
		OmnAlarm << "Failed get subiil: " << cur_iilidx << enderr;
		return;
	}

	if (!subiil->verifyIdx(cur_idx, cur_value, cur_docid))
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}

	if (reverse)
	{
		prevQueryPos(cur_idx, cur_iilidx);
	}
	else
	{
		nextQueryPos(cur_idx, cur_iilidx);
	}
	return;
}


void
AosIILBigStr::nextIndexFixErrorIdxSingle(
		int &cur_idx,
		int &cur_iilidx,
		const OmnString &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	aos_assert(cur_idx >= 0 && cur_iilidx >=0);
	has_data = true;

	// 1. index out of bound
	if (cur_iilidx > 0 || cur_idx > mNumDocs -1)
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}

	// 2. value changed
	// SHAWN_WARN
	// if (mValues[cur_idx] != cur_value && mDocids[cur_idx] != cur_docid)
	if (cur_value != mValues[cur_idx] && mDocids[cur_idx] != cur_docid)
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}

	// 3. nothing changed		
	if (reverse)
	{
		prevQueryPos(cur_idx,cur_iilidx);
	}
	else
	{
		nextQueryPos(cur_idx,cur_iilidx);
	}
	return;
}


bool
AosIILBigStr::verifyIdx(
		const int &cur_idx,
		const OmnString &cur_value,
		const u64 &cur_docid)
{
// shawn query function , single
	// SHAWN_WARN
	// return (cur_idx < mNumDocs && 
	// 		mDocids[cur_idx] == cur_docid && 
	// 		mValues[cur_idx] == cur_value);
	return (cur_idx < mNumDocs && 
			mDocids[cur_idx] == cur_docid && 
			cur_value == mValues[cur_idx]);
}


bool	
AosIILBigStr::copyData(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
				const int &start_idx,
				const int &start_iilidx,
				const int &end_idx,
				const int &end_iilidx,
				const AosRundataPtr &rdata)
{
// shawn query function root function
	if (query_rslt && query_bitmap)
	{
		return copyDataWithCheck(query_rslt,query_bitmap,start_idx,start_iilidx,end_idx,end_iilidx,rdata);
	}
	
	bool reverse = false;
	if (query_rslt && query_rslt->isReverse()) reverse = true;
	if (reverse)
	{
		query_rslt->setDataReverse(true);
	}
	if (isSingleIIL())
	{
		return copyDataSingle(
					query_rslt,
					query_bitmap,
					start_idx,
					end_idx,
					rdata);
	}
	
	// rootIIL
	int start = 0;
	int end = 0;
	for(int i = start_iilidx;i <= end_iilidx;i++)
	{
		start = 0;
		end = mNumEntries[i] -1;
		if (i == start_iilidx) start = start_idx;
		if (i == end_iilidx) end = end_idx;
		
		if (i == 0)
		{
			copyDataSingle(
				query_rslt,
				query_bitmap,
				start,
				end,
				rdata);
		}
		else
		{
			getIILByIndex(i,true,rdata)->copyDataSingle(
				query_rslt,
				query_bitmap,
				start,
				end,
				rdata);
		}
	}
		
	return true;
}


bool	
AosIILBigStr::copyDataWithCheck(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
				const int &start_idx,
				const int &start_iilidx,
				const int &end_idx,
				const int &end_iilidx,
				const AosRundataPtr &rdata)
{
// shawn query function
	bool reverse = query_rslt->isReverse();
	bool keep_search = true;
	if (isSingleIIL())
	{
		return copyDataSingleWithCheck(
					query_rslt,
					query_bitmap,
					start_idx,
					end_idx,
					reverse,
					keep_search,
					rdata);
	}

	int start = 0;
	int end = 0;
	if (reverse)
	{
		for(int i = end_iilidx;i >= start_iilidx;i--)
		{
			start = 0;
			end = mNumEntries[i] -1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;
			copyDataSingleWithCheck(
				query_rslt,
				query_bitmap,
				start,
				end,
				reverse,
				keep_search,
				rdata);
			if (query_rslt->isFull())
			{
				return true;
			}
		}
		if (keep_search)
		{
			query_rslt->setIndex(0);
			query_rslt->setIILIndex(0);
			query_rslt->setFinished(true);
		}
	}
	else //normal order
	{
		for(int i = start_iilidx;i <= end_iilidx;i++)
		{
			start = 0;
			end = mNumEntries[i] -1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;
			copyDataSingleWithCheck(
				query_rslt,
				query_bitmap,
				start,
				end,
				reverse,
				keep_search,
				rdata);
			if (query_rslt->isFull())
			{
				return true;
			}
		}
		if (keep_search)
		{
			query_rslt->setIndex(0);
			query_rslt->setIILIndex(0);
			query_rslt->setFinished(true);
		}
	}
	return true;
}


bool	
AosIILBigStr::copyDataSingleWithCheck(
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const int &start_idx,
			const int &end_idx,
			const bool reverse, 
			bool  keep_search,
			const AosRundataPtr &rdata)
{
	return false;
}			


bool	
AosIILBigStr::copyDataSingle(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
				const int &start_idx,
				const int &end_idx,
				const AosRundataPtr &rdata)
{
	int size = end_idx - start_idx +1;
	aos_assert_r(size >= 0,false);
	if (size == 0)return true;

	if (query_rslt)
	{
		query_rslt->appendBlock(&(mDocids[start_idx]),size);
	}
	
	if (query_bitmap)
	{
		query_bitmap->appendBlock(&(mDocids[start_idx]),size);
	}
	return true;
}


bool	
AosIILBigStr::nextQueryPos(int &idx,int &iilidx)
{
// shawn query function, rewrite
	if (iilidx < 0)
	{
		iilidx = 0;
		idx = 0;
		return true;
	}
	idx ++;
	if (isRootIIL() && idx >= mNumEntries[iilidx])
	{
		iilidx ++;
		idx = 0;
	}
	
	return true;
}


bool	
AosIILBigStr::prevQueryPos(int &idx,int &iilidx)
{
// shawn query function, rewrite
	if (isSingleIIL() && iilidx > 0)
	{
		iilidx = 0;
		idx = mNumDocs -1;
		return true;
	}

	idx --;

	if (isRootIIL() && idx < 0)
	{
		iilidx --;
		aos_assert_r(iilidx >= 0,false);
		idx = mNumEntries[iilidx] -1;
	}
	
	return true;
}


bool 
AosIILBigStr::batchQueryPriv(
		const AosBatchQueryCtlrPtr &query,
		const AosOpr &opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// Batch querying is used to query large IILs. Its main purpose is
	// to create segments for the condition [opr, value]. The segments
	// are stored in 'query_rslt'. 
	// 
	if (isSingleIIL())
	{
		return batchQuerySinglePriv(query, opr, value, rdata);
	}

	if (opr == eAosOpr_ne)
	{
		return batchQueryNEPriv(query, value, rdata);
	}
	else if (opr == eAosOpr_like)
	{
		return batchQueryLikePriv(query, value, rdata);
	}
	
	bool rslt = false;
	int idx_start = -10;
	int iilidx_start = -10;
	int idx_end = -10;
	int iilidx_end = -10;
	u64 docid;
	bool isunique = false;
	
	if (opr == eAosOpr_an)
	{
		// It needs to retrieve all the IILs.
		OmnNotImplementedYet;
		return false;
	}
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value, docid, isunique, rdata);
	if (!rslt || idx_start < 0 || iilidx_start < 0)
	{
		// This means that it did not find anything.
		query->noResults();
		return true;
	}
	
	aos_assert_r(iilidx_start >= 0, false);
	rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value, docid, isunique, rdata);
	if (!rslt || idx_end < 0 || iilidx_end < 0)
	{
		OmnAlarm << "Internal error: " << rslt << ":"
			 << idx_end << ":" << iilidx_end << enderr;
		query->setError(__FILE__, __LINE__, "Internal error");
		return false;
	}

	// Need to create batch query based on the matched iils.
	OmnNotImplementedYet;
	return false;
}


bool 
AosIILBigStr::batchQuerySinglePriv(
		const AosBatchQueryCtlrPtr &query, 
		const AosOpr &opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosIILBigStr::batchQueryNEPriv(
		const AosBatchQueryCtlrPtr &query, 
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosIILBigStr::batchQueryLikePriv(
		const AosBatchQueryCtlrPtr &query, 
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
*/



