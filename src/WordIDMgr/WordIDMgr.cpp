////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Header Format:
// 		IILType		u8
// 		IILID		u64
// 		WordID		u64
// 		Num Docs	u32
// 		Flag		char
// 		Hit Count	u32
// 		Version		u32
//		IIL Size	u32
//		Offset		u64
//		Seqno		u32
//		Comp Size	u32
//		Comp Offset u64
//		Comp Seqno	u32
//
// Modification History:
// 	Created: 10/26/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordIDMgr/WordIDMgr.h"

#include "Debug/ExitHandler.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"

AosWordIDMgr::AosWordIDMgr()
:
mCurWordID(1),
mLock(OmnNew OmnMutex()),
mRundata(OmnNew AosRundata())
{
//	mWordArray = OmnNew AosBuffArrayVar(false);
	mIDArray = OmnNew AosBuffArrayVar(false);
	mTmpValue_a = OmnNew AosBuff(eMaxBuffLen);
	mTmpPtrA	= mTmpValue_a->data();
	mTmpPtrA2 	= (u64*)mTmpPtrA;
	mTmpValue_b = OmnNew AosBuff(eMaxBuffLen);
	mTmpPtrB	= mTmpValue_b->data();
	mTmpPtrB2 	= (u64*)mTmpPtrB;

	OmnString str;
	str << "<zky_buffarray zky_stable = \"false\" >"
		<< "<CompareFun cmpfun_alphabetic=\"false\" cmpfun_reserve=\"false\" cmpfun_type=\"str\" cmpfun_size=\"24\" offset=\"24\"></CompareFun>"
		<<  "</zky_buffarray>"; 


	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert(cmp_tag);
	mWordArray = AosBuffArrayVar::create(cmp_tag, mRundata);
	
	mIDArray->insertEntry(0,"                                                             ",28,mRundata.getPtr());





}

AosWordIDMgr::~AosWordIDMgr() 
{
}

void
AosWordIDMgr::lock()
{
	mLock->lock();
}

void
AosWordIDMgr::unlock()
{
	mLock->unlock();
}

OmnString 
AosWordIDMgr::getWord(const u32 &word_id)
{
	if(word_id >= mIDArray->getNumEntries())
	{
		return "";
	}
	if(((u64*)(mIDArray->getEntry(word_id)))[0] == 0)
	{
		return "";
	}
	return OmnString(mIDArray->getEntry(word_id)+eIDArrayWordOffset);
}

u32 
AosWordIDMgr::getWordIDSafe(const OmnString &word)
{
	mLock->lock();
	u32 id = getWordIDPriv(word);
	mLock->unlock();
	return id;
}

u32 
AosWordIDMgr::getWordIDPriv(const OmnString &word)
{ 	
    // 1、搜索整个array1。
    bool unique;
    i64 index;
    index = mWordArray->findPos(0,word.data()-eWordArrayWordOffset,word.length()+eWordArrayWordOffset,unique);

	// 2、if如果找到匹配字符串，输出对应WordID；return;
	if(!unique)
	{
		char* data;
		int len;
		bool rslt = mWordArray->getEntry(index, data, len);
		aos_assert_r(rslt,false);
		return *((u64*)data);
	}
	
   	// 3.如果未找到匹配字符串,
   	return createWord(word, index);
   	
}

u32 
AosWordIDMgr::createWord(const OmnString &word, const i64 &index)
{
	//	1. 重新分配一个新的WordID
	u64 wordid = generateWordID();
	int num_nodes = mWordArray->getNumEntries();

	//	2. 拼装除了sortid以外的所有内容
	u32 buff_len1 = word.length()+1;
	u32 buff_len2 = buff_len1;

	mTmpPtrA2[0] = wordid;
	mTmpPtrA2[2] = OmnGetSystemTime();
	aos_assert_r(buff_len1 <= eMaxStrLen,false);
	memcpy(mTmpPtrA+eWordArrayWordOffset, word.data(),buff_len1);
	buff_len1 += eWordArrayWordOffset;

	memcpy(mTmpPtrB+eIDArrayWordOffset, word.data(),buff_len2);
	buff_len2 += eIDArrayWordOffset;

	// handle all about the sort id;
	i64 modify_idx = -1;
	bool direct = true;
OmnScreen << "====================" << endl;
for(int i = 0;i < num_nodes;i++)
{
OmnScreen << ((u64*)mWordArray->getEntry(i))[0] << endl;
//OmnScreen << OmnString(mIDArray->getEntry(i)+eIDArrayWordOffset) << endl;
}
OmnScreen << "====================" << endl;

	if(num_nodes > 1)
	{
		//	(buffArrayVar已完成）mWordArray中以word，用二分法查找位置
		// 	将word和新的WordID,timestamp插入array1；
		if(index == 0)
		{
			mTmpPtrA2[1] = 0;
			mTmpPtrB2[0] = 0;
			modify_idx = 1;
			direct = true;
		}
		else if(index == num_nodes)
		{ 
			mTmpPtrA2[1] = eMaxSortID;
			mTmpPtrB2[0] = eMaxSortID;
			modify_idx = num_nodes-1;
			direct = false;
		}
		else
		{
			// in the middle
			
			u64 front = ((u64*)mWordArray->getEntry(index-1))[1];
			u64 end = ((u64*)(mWordArray->getEntry(index)))[1];
	
			aos_assert_r(end > front,false);
			
			if(end - front == 1)
			{
				if(end < eMaxSortID/2)
				{
					mTmpPtrA2[1] = end;
					mTmpPtrB2[0] = end;
					modify_idx = index +1;
					direct = true;
				}
				else
				{
					mTmpPtrA2[1] = front;
					mTmpPtrB2[0] = front;
					modify_idx = index -1;
					direct = false;				
				}
			}
			else
			{
				mTmpPtrA2[1] = (front+end)/2;
			}
		}

	}
	else if (num_nodes == 1)
	{
		if(index == 0)
		{
			mTmpPtrA2[1] = 0;
			mTmpPtrB2[0] = 0;
			
			//原来那个还要更新
			((u64*)(mWordArray->getEntry(0)))[1] = eMaxSortID;
			((u64*)(mWordArray->getEntry(0)))[2] = OmnGetSystemTime();

			u64 tmp_word_id = ((u64*)(mWordArray->getEntry(0)))[0]; 
			((u64*)(mIDArray->getEntry(tmp_word_id)))[0] = eMaxSortID;
			
		}	
		else
		{
			mTmpPtrA2[1] = eMaxSortID;
			mTmpPtrB2[0] = eMaxSortID;
		}
	}
	else // num_nodes == 0
	{
		mTmpPtrA2[1] = 0;
		mTmpPtrB2[0] = 0;
	}

	mWordArray->insertEntry(index,mTmpPtrA,buff_len1,mRundata.getPtr());
	mIDArray->insertEntry(wordid,mTmpPtrB,buff_len2,mRundata.getPtr());
OmnScreen << "====================" << endl;
for(int i = 0;i < num_nodes+1;i++)
{
//OmnScreen << OmnString(mIDArray->getEntry(i)+eIDArrayWordOffset) << endl;
OmnScreen << ((u64*)mWordArray->getEntry(i))[0] << endl;
}
OmnScreen << "====================" << endl;


	while(modify_idx >= 0)
	{
		if((u64)modify_idx == 0 || (u64)modify_idx == num_nodes)
		{
			OmnAlarm << "Can not change node :" << modify_idx <<enderr;
			return false;
		}

		u64 front = ((u64*)(mWordArray->getEntry(modify_idx-1)))[1];
		u64 end = ((u64*)(mWordArray->getEntry(modify_idx+1)))[1];
		
		if(end - front == 1)
		{
			if(direct)
			{
				// sort id update to end
				((u64*)(mWordArray->getEntry(modify_idx)))[1] = end;
				((u64*)(mWordArray->getEntry(modify_idx)))[2] = OmnGetSystemTime();
				u32 tmp_word_id = ((u64*)(mWordArray->getEntry(modify_idx)))[0];
				((u64*)(mIDArray->getEntry(tmp_word_id)))[0]   = end;
			modify_idx ++;
			}
			else
			{
				// sort id update to front
				((u64*)(mWordArray->getEntry(modify_idx)))[1] = front;
				((u64*)(mWordArray->getEntry(modify_idx)))[2] = OmnGetSystemTime();
				u32 tmp_word_id = ((u64*)(mWordArray->getEntry(modify_idx)))[0];
				((u64*)(mIDArray->getEntry(tmp_word_id)))[0] = front;
				modify_idx --;
				direct = false;				
			}
		}
		else
		{
			// sort id update to (end+front)/2
			u64 tmp_sort_id = (end+front)/2;
			u32 tmp_word_id = ((u64*)(mWordArray->getEntry(modify_idx)))[0];
			((u64*)(mWordArray->getEntry(modify_idx)))[1] = tmp_sort_id;
			((u64*)(mWordArray->getEntry(modify_idx)))[2] = OmnGetSystemTime();
			((u64*)(mIDArray->getEntry(tmp_word_id)))[0] = tmp_sort_id;
			modify_idx = -1;
		}
	}

OmnScreen << "====================" << endl;
for(int i = 0;i < num_nodes+1;i++)
{
//OmnScreen << OmnString(mIDArray->getEntry(i)+eIDArrayWordOffset) << endl;
OmnScreen << ((u64*)mWordArray->getEntry(i))[0] << endl;
}
OmnScreen << "====================" << endl;
OmnScreen << "*******************************************"<< endl;

	return wordid;	
}	

bool 
AosWordIDMgr::saveWordMgr(const AosBuffPtr &buff)
{
	bool rslt = false;

	rslt = mWordArray->setDataToBuff(buff);
	aos_assert_r(rslt,false);

	rslt = mIDArray->setDataToBuff(buff);
	aos_assert_r(rslt,false);

	return rslt;
}

bool 
AosWordIDMgr::loadWordMgr(const AosBuffPtr &buff)
{
	bool rslt = false;

	rslt = mWordArray->getDataFromBuff(buff);
	aos_assert_r(rslt,false);

	rslt = mIDArray->getDataFromBuff(buff);
	aos_assert_r(rslt,false);

	return rslt;
}

void 
AosWordIDMgr::clear()
{
	bool rslt = false;

	rslt = mWordArray->reset();
	aos_assert(rslt);

	rslt = mIDArray->reset();
	aos_assert(rslt);

	return; 
}

bool 
AosWordIDMgr::exportData(const u64 &tsmp, u64 &cur_tsmp, const AosBuffPtr &buff)
{
	mLock->lock();
/*      1、check every entry in array1,
		for(.....)
		{
			 if the timestamp > tsmp ,put it into buff, else do nothing
		}
	  2. set current system timestamp to cur_tsmp
		return;
// 将获取的时间戳与当前存储在list1中的时间戳作比较；
*/      
	u64 num= mWordArray->getNumEntries();
	u64 timestamp = 0;
	for(u64 i=0;i< num; i++)
	{
	 /*	get timestamp
             
		compare with tsmp
		if > tsmp, set to buff
		buff->setU64(1111);
		1. wordid
		2. sortid
		3. timestamp
		4. word
setU32
setU64
setStr   */
		timestamp=((u64*)mWordArray->getEntry(i))[2];
       if (timestamp > tsmp)
       {
         buff->setU32(((u64*)mWordArray->getEntry(i))[0]);
         buff->setU64(((u64*)mWordArray->getEntry(i))[1]);
         buff->setU64(((u64*)mWordArray->getEntry(i))[2]);
         buff->setOmnStr(OmnString(mWordArray->getEntry(i)+eWordArrayWordOffset));
       }

 
            
         
	}
	buff->setU32(0);
        cur_tsmp=OmnGetSystemTime();        


	mLock->unlock();
	return true;
}

u64 
AosWordIDMgr::getTimeStamp()
{
	
	return mClientTimeStamp;
}


bool 
AosWordIDMgr::importData(const AosBuffPtr &buff, const u64 &new_tsmp)
{      

	u32 i=0;
	u32 wordid;
	u64 sortid;
	u64 timestamp;
	OmnString word;
        


/*	while(1)
	{	     
	
		wordid = buff->getU32(0);
		if(wordid==0)
		{
			break;
		}                   

		sortid=buff->getU64(0);
		timestamp=buff->getU64(0);
		word=buff->getOmnStr("");

        ((u64*)(mWordArray->getEntry(i)))[0] = wordid;
        ((u64*)(mWordArray->getEntry(i)))[1] = sortid;
        ((u64*)(mWordArray->getEntry(i)))[2] =timestamp;
        ((u64*)(mWordArray->getEntry(i)))[3] = word;
                
        ((u64*)(mIDArray->getEntry(i)))[0] = wordid;
        ((u64*)(mIDArray->getEntry(i)))[1]=sortid;
        ((u64*)(mIDArray->getEntry(i)))[2]=word;   

                i++;



	}
*/
	mClientTimeStamp = new_tsmp;
	return true;
}


int 
AosWordIDMgr::compareWord(const u32 &id1, const u32 &id2)
{
     //1、通过WoldID,即id1，id2,借用array2,取出sortid1,sortid2;
     //2、比较sortid1,sortid2的大小，返回具有较大的SortID的WorldID的值 
	i64 rslt = ((u64*)(mIDArray->getEntry(id1)))[0] - ((u64*)(mIDArray->getEntry(id2)))[0];
	if(rslt > 0)return 1;
	if(rslt < 0)return -1;
	return 0;
}

u64 
AosWordIDMgr::generateWordID()
{
	return mCurWordID++;
}

