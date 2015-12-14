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
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/HashedObj.h"

#include "Rundata/Rundata.h"
#include "ReliableFile/ReliableFile.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "UtilHash/StrObjHash.h"


AosBuffPtr 
AosHashedObj::getInsertBucket(
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		const u64 &offset, 
		const int contentsLen, 
		const int bucket_size,
		u64 &insertPos,
		const AosRundataPtr &rdata)
{
	// A bucket may contain too many entries to fit into one bucket. In 
	// that case, additionl records are allocated for the bucket. These
	// records are linked through the ContinueField. This function reads
	// the last record and returns the position that points to the
	// ContinueField of the last record. This is the position a new
	// value pair can be inserted.
	//
	// Note: In the current implementations, records are not sorted. This
	// will be re-implemented so that records are sorted. 
	//
	// Bucket format is:
	// 		length			(u32)
	// 		front-poison	(u32)
	// 		contents		(variable)
	// 		continue_field	(u64)
	// 		end-poison		(u32)
	// 		length			(u32)
	aos_assert_r(file, 0);
	int guard = eMaxNumRecords;
	u64 local_offset = offset;
	mLock->lock();
	AosBuffPtr buff = OmnNew AosBuff(bucket_size, 0 AosMemoryCheckerArgs);
	while (guard--)
	{
		if(local_offset < file->getLength())
		{
			bool rslt = file->readToBuff(local_offset, bucket_size, buff->data(), rdata.getPtr());
			aos_assert_rl(rslt, mLock, 0);
		}
		//bool rslt = file->readToBuff(local_offset, bucket_size, buff->data(), rdata);
		buff->setDataLen(bucket_size);
		buff->reset();
		//aos_assert_rl(rslt, mLock, 0);
		u32 bucket_len = buff->getU32(0);
		if (isEmptyBucket(*(buff.getPtr()), bucket_len, bucket_size))
		{
			buff->reset();
			buff->setU32(eEmptyRecordSize);
			buff->setU32(eFrontPoison);
			buff->setOmnStr("");
			buff->setU64(0);
			buff->setU32(eEndPoison);
			buff->setU32(eEmptyRecordSize);
			bool rslt = file->put(local_offset, buff->data(), bucket_size, false, rdata.getPtr());
			aos_assert_rl(rslt, mLock, 0);
			buff->setCrtIdx(eLengthSize + ePoisonSize);
			insertPos = local_offset;
			mLock->unlock();
			return buff;
		}

		aos_assert_rl(bucket_len >= eEmptyRecordSize, mLock, 0);
		aos_assert_rl(bucket_len <= (u32)bucket_size, mLock, 0);

		buff->setCrtIdx(bucket_len - getContinueOffset());
		u64 continue_field = buff->getU64(0);

		int space_left = bucket_size - (int)bucket_len;
		if (space_left >= contentsLen)
		{
			// There is enough space
			buff->setCrtIdx(bucket_len - getEndSizeOffset());
			insertPos = local_offset;
			mLock->unlock();
			return buff;
		}

		if (continue_field == 0)
		{
			// This is the last bucket record. Need to check how much space left in 
			// the current bucket record. 
			// There is no enough space. Need to allocate a new bucket.  
			buff = allocateNewBucket(file, local_offset, bucket_len, bucket_size, rdata.getPtr());
			aos_assert_rl(buff, mLock, 0);
			buff->setCrtIdx(eLengthSize + ePoisonSize); 
			insertPos = local_offset;
			mLock->unlock();
			return buff;
		}
		
		local_offset = continue_field;
	}
	mLock->unlock();
	OmnShouldNeverComeHere;
	return 0;
}


AosBuffPtr
AosHashedObj::allocateNewBucket(
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		u64 &offset, 
		const int bucket_len,
		const int bucket_size,
		const AosRundataPtr &rdata)
{
	static int lsAdditionBuckets = 0;

	lsAdditionBuckets++;
	OmnScreen << "Added additional bucket: " << lsAdditionBuckets << endl;
	// This function creates a new bucket record for the current bucket record.
	// The current bucket record is at 'offset' and its length is 'bucket_len'.
	// This function update the existing bucket's continuation field.
	//
	// 		length			(u32)
	// 		front-poison	(u32)
	// 		contents		(variable)
	// 		empty string	(sizeof(int))
	// 		continue_field	(u64)
	// 		end-poison		(u32)
	// 		length			(u32)
	//
	AosBuffPtr buff = OmnNew AosBuff(bucket_size, 0 AosMemoryCheckerArgs);
	buff->setU32(eEmptyRecordSize);
	buff->setU32(eFrontPoison);
	buff->setOmnStr("");
	buff->setU64(0);
	buff->setU32(eEndPoison);
	buff->setU32(eEmptyRecordSize);
	
	u64 crt_file_size = file->readU64(AosStrObjHash::eFileSizePos, 0, rdata.getPtr());
	aos_assert_r(crt_file_size, 0);
	bool rslt = file->put(crt_file_size, buff->data(), bucket_size, false, rdata.getPtr());
	aos_assert_r(rslt, 0);

	// Save the continue field
	u64 pos = offset + bucket_len - eLengthSize - ePoisonSize - eContinueSize;
	rslt = file->setU64(pos, crt_file_size, false, rdata.getPtr());

	// Update 'offset' to point to the new location
	offset = crt_file_size;

	// Update the current file size
	crt_file_size += bucket_size;
	file->setU64(AosStrObjHash::eFileSizePos, crt_file_size, true, rdata.getPtr());
	return buff;
}
		

bool 
AosHashedObj::insertData(
		const AosBuffPtr &buff, 
		const int contentsLen,
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		const u64 &offset,
		const AosRundataPtr &rdata)
{
	// This function assumes 'buff' is a modified buff, containing the 
	// new contents. This function will modify the length fields (both
	// front and back) by adding the delta 'contentsLen' to them. 
	// It then saves the contents at the position 'offset'.
	aos_assert_r(file, false);
	aos_assert_r(contentsLen >= 0, false);
	mLock->lock();

	buff->reset();
	int len = buff->getU32(0);

	buff->backU32();
	buff->setU32(len + contentsLen);

	// Test: get the length
	//      length          (u32)
	//      front-poison    (u32)
	//      contents        (variable)
	//      empty string    (sizeof(int
	//      continue_field  (u64)
	//      end-poison      (u32)
	//      length          (u32)
	buff->reset();

	/*
	OmnScreen << "Bucket len: " << buff->getU32(0) << endl;
	OmnScreen << "front_Pos: " << buff->getU32(0) << endl;
	OmnScreen << "key: " << buff->getOmnStr("") << endl;
	OmnScreen << "body: " << buff->getU64(0) << endl; 
	OmnScreen << "real length: " << len + contentsLen << endl;
	*/
	
	buff->setCrtIdx(len + contentsLen - eLengthSize);
	buff->setU32(len + contentsLen);
	/*
	OmnScreen << "^^^^^^^^^^^^^^^^^^^^^ add ^^^^^^^^^^^^^^^^^^^^^^^" << endl;
	buff->reset();
	OmnString key;
	OmnScreen << "The Priv length: " << len << endl;
	OmnScreen << "The add length: " << contentsLen << endl;
	
	OmnScreen << "Bucket len: " << buff->getU32(0) << endl;
	OmnScreen << "front_Pos: " << buff->getU32(0) << endl;
	while ((key = buff->getOmnStr("")) != "")
	{
		OmnScreen << "key: " << key << endl;
		OmnScreen << "body: " << buff->getU64(0) << endl; 
	}
	//buff->setCrtIdx(buff->getCrtIdx()-sizeof(int));
	OmnScreen << "continue field: " << buff->getU64(0) << endl;
	OmnScreen << "poision: " << buff->getU32(0) << endl;
	OmnScreen << "bucket length: " << buff->getU32(0) << endl;
	OmnScreen << "^^^^^^^^^^^^^^^^^^^^^ add ^^^^^^^^^^^^^^^^^^^^^^^" << endl;
	*/
	bool rslt = file->put(offset, buff->data(), buff->dataLen(), true, rdata.getPtr());
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool 
AosHashedObj::readObj(
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		const u64 &offset, 
		const OmnString &key, 
		const int bucket_size, 
		AosHashedObjPtr &obj,
		const AosRundataPtr &rdata)
{
	// It reads the bucket pointed to by 'offset'. If the bucket contains the matching
	// entry (by 'key'), it creates an instance of the hashed obj (by cloning mDftObj),
	// and reads in all the information. Otherwise, it returns false.
	//
	// Note: In the current implementation, it does the linear searching. It will be
	// re-implemented through binary search in case there are many entries in a bucket.
	//
	// Bucket format is:
	// 		bucket length 	(u32)
	// 		front_poison	(u32)
	// 		data			(variable)
	// 		data
	// 		...
	// 		continue_field	(u64)
	//		end_poison		(u32)
	//		bucket length 	(u32)
	
	aos_assert_r(file, false);
	obj = 0;

	// Chen Ding, 2015/05/25
	// char mem[bucket_size];
	// AosBuff buff(mem, bucket_size, 0 AosMemoryCheckerArgs);
	AosBuff buff(AosMemoryCheckerArgsBegin);

	bool isEmpty;
	bool rslt = readToBuff(buff, isEmpty, file, offset, bucket_size, rdata.getPtr());
	if (!rslt) return false;
	if (isEmpty) return false;

	OmnString kk;
	int guard = eMaxNumRecords;
	while (guard--)
	{
		while ((kk = buff.getOmnStr("")) != "")
		{
			const int len = doesRecordMatch(buff, kk, key, obj);
			if (obj)
			{
				// Found the entry
				aos_assert_r(key == kk, false);
				obj->setKeyStr(key);
				aos_assert_r(len > 0, false);
				return true;
			}
		}

		u64 continue_flag = buff.getU64(0);
		if (continue_flag == 0) 
		{
			// Did not find the entry
			obj = 0;
			return false;
		}

		// It is continued at 'continue_flag'.
		bool rslt = readToBuff(buff, isEmpty, file, continue_flag, bucket_size, rdata.getPtr());
		aos_assert_r(rslt, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosHashedObj::deleteObj(
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		const u64 &offset, 
		const OmnString &key, 
		const int bucket_size,
		const AosRundataPtr &rdata) 
{
	// It reads the bucket pointed to by 'offset'. If the bucket contains the matching
	// entry (by 'key'), it delete ifself. Otherwise, it returns false.
	//
	// Note: In the current implementation, it does the linear searching. It will be
	// re-implemented through binary search in case there are many entries in a bucket.
	//
	// Bucket format is:
	// 		bucket length 	(u32)
	// 		front_poison	(u32)
	// 		data			(variable)
	// 		data
	// 		...
	// 		continue_field	(u64)
	//		end_poison		(u32)
	//		bucket length 	(u32)
	
	aos_assert_r(file, false);

	// Chen Ding, 2015/05/25
	char mem[bucket_size];
	AosBuff buff(mem, bucket_size, 0, false AosMemoryCheckerArgs);
	bool isEmpty;
	u64 local_offset = offset;
	bool rslt = readToBuff(buff, isEmpty, file, local_offset, bucket_size, rdata.getPtr());
	if (!rslt) return false;
	if (isEmpty) return false;

	OmnString kk;
	int guard = eMaxNumRecords;
	while (guard--)
	{
		int entryStartPos = buff.getCrtIdx();
		while ((kk = buff.getOmnStr("")) != "")
		{
			AosHashedObjPtr obj(this, false);
			const int len = doesRecordMatch(buff, kk, key, obj);
			if (!obj)
			{
				entryStartPos = buff.getCrtIdx();
				continue;
			}
			// Found the entry
			// old bucket length.
			//aos_assert_r(len > 0, false);
			aos_assert_r(len < bucket_size, false);
			buff.reset();
			int bucket_len = buff.getU32(0);
			aos_assert_r(bucket_len > 0, false);
			aos_assert_r(bucket_len <= bucket_size, false);

			//content length
			const int contentsLen = mKey.length() + sizeof(int) + len;
			aos_assert_r(contentsLen < bucket_size, false);

			// move size
            int moveSize = bucket_len - entryStartPos - contentsLen;
            aos_assert_r(moveSize > 0, false);
            aos_assert_r(moveSize < bucket_len, false);
            memmove(&mem[entryStartPos], &mem[entryStartPos + contentsLen], moveSize);

            // reset head length
            buff.reset();
            buff.setU32(bucket_len - contentsLen);

            // reset tail length
            buff.setCrtIdx(bucket_len - contentsLen - eLengthSize);
            buff.setU32(bucket_len - contentsLen);

            // write to file
            file->put(local_offset, mem, bucket_size, true, rdata.getPtr());

			return true;
		}

		local_offset = buff.getU64(0);
		if (local_offset == 0) 
		{
			// Did not find the entry
			return false;
		}

		// It is continued at 'continue_flag'.
		bool rslt = readToBuff(buff, isEmpty, file, local_offset, bucket_size, rdata.getPtr());
		aos_assert_r(rslt, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosHashedObj::readToBuff(
            AosBuff &buff,
			bool  &isEmpty,
	    	//const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
            const u64 &offset,
			const int bucket_size,
			const AosRundataPtr &rdata)
{
	aos_assert_r(file, false);
	aos_assert_r(bucket_size > 0, false);
	// Ketty 2012/11/29
	u64 file_size = file->getLength();
	if(offset >= file_size)
	{
		isEmpty = true;
		return false;
	}
	// Ketty 2012/11/29
	/*
	int num = file->readToBuff(offset, buff.buffLen(), buff.data(), rdata);
	if (num < 0)
	{
		isEmpty = true;
		return false;
	}
    // aos_assert_r(buff.buffLen() == num, false);
	buff.setDataLen(num);
	*/
	bool rslt = file->readToBuff(offset, buff.buffLen(), buff.data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	buff.setDataLen(buff.buffLen());
	// Ketty end.

	/*
	buff.reset();
	OmnScreen << "^^^^^^^^^^^^^^^^^^^^^ delete ^^^^^^^^^^^^^^^^^^^^^^^" << endl;
	OmnString key;
	OmnScreen << "Bucket len: " << buff.getU32(0) << endl;
	OmnScreen << "front_Pos: " << buff.getU32(0) << endl;
	while ((key = buff.getOmnStr("")) != "")
	{
		OmnScreen << "key: " << key << endl;
		OmnScreen << "body: " << buff.getU64(0) << endl; 
	}
	//buff.setCrtIdx(buff.getCrtIdx()-sizeof(int));
	OmnScreen << "continue field: " << buff.getU64(0) << endl;
	OmnScreen << "poision: " << buff.getU32(0) << endl;
	OmnScreen << "bucket length: " << buff.getU32(0) << endl;
	OmnScreen << "^^^^^^^^^^^^^^^^^^^^^ delete ^^^^^^^^^^^^^^^^^^^^^^^" << endl;
	*/	
	buff.reset();
	u32 bucket_len = buff.getU32(0);
	isEmpty = isEmptyBucket(buff, bucket_len, bucket_size);
	if (isEmpty) 
	{
		return true;
	}
	
	aos_assert_r(bucket_len <= (u32)bucket_size, false);
	buff.setDataLen(bucket_len);
	buff.setCrtIdx(eLengthSize + ePoisonSize);
	return true;
}


bool 
AosHashedObj::addObj(
	    	//const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const u64 &offset,
			const int bucket_size,
			const AosRundataPtr &rdata)
{
	// It adds 'this' object to the hash in the bucket pointed to by 'offset'. 
	// It first creates a buff, then it knows how long the object is. 
	aos_assert_r(file, false);
	u64 local_insertPos = 0;

	// Chen Ding, 2015/05/25
	char mem[bucket_size];
	AosBuff local_buff(mem, bucket_size, 0, false AosMemoryCheckerArgs);
	local_buff.setOmnStr(mKey);
	int len  = setContentsToBuff(local_buff);
	len = local_buff.dataLen();
	aos_assert_r(len < bucket_size, false);
	
	AosBuffPtr buff = getInsertBucket(file, offset, len, bucket_size, local_insertPos, rdata.getPtr());
	aos_assert_r(local_insertPos > 0, false);
	aos_assert_r(buff, false);

	int startCrtIdx = buff->getCrtIdx();

	memmove(&(buff->data())[buff->getCrtIdx() + len], 
			&(buff->data())[buff->getCrtIdx()], getEndSizeOffset());

	//memmove(&(buff->data())[buff->getCrtIdx()], mem, len); 
	memcpy(&(buff->data())[buff->getCrtIdx()], mem, len); 
	buff->setCrtIdx(startCrtIdx + len);

	bool rslt = insertData(buff, len, file, local_insertPos, rdata.getPtr());
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosHashedObj::modifyContentsToFile(
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		const u64 &offset, 
		const int bucket_size,
		const AosRundataPtr &rdata)
{
	// This function saves the entry, which means that the entry
	// should be in the bucket. Otherwise, it is an error. It also
	// assumes this object holds valid values, including its key 
	// and value. 
	//
	// Note: In the current implementation, it does the linear searching. It will be
	// re-implemented through binary search in case there are many entries in a bucket.
	//
	// Bucket format is:
	// 		bucket length 	(u32)
	// 		front_poison	(u32)
	// 		key				(string)
	// 		data			(variable, the rest of the entry)
	// 		key				(string)
	// 		data			(variable, the rest of the entry)
	// 		...
	// 		empty string	(4 bytes)
	// 		continue_field	(u64)
	//		end_poison		(u32)
	
	AosHashedObjPtr obj = 0;

	// Chen Ding, 2015/05/25
	// char mem[bucket_size]; 
	// AosBuff buff(mem, bucket_size, 0 AosMemoryCheckerArgs);
	AosBuff buff(AosMemoryCheckerArgsBegin);
	bool isEmpty;
	bool rslt = readToBuff(buff, isEmpty, file, offset, bucket_size, rdata.getPtr());
	aos_assert_r(rslt, false);
	if (isEmpty) return false;

	OmnString kk;
	int guard = eMaxNumRecords;
	u64 crt_offset = offset;
	while (guard--)
	{
		while ((kk = buff.getOmnStr("")) != "")
		{
			int pos = buff.getCrtIdx();
			
			doesRecordMatch(buff, kk, mKey, obj);
			if (obj)
			{
				// Found the entry. Need to modify the record.
				//AosBuff oldbuff(eBuffInitSize, eBuffIncSize AosMemoryCheckerArgs); 
				//AosBuff newbuff(eBuffInitSize, eBuffIncSize AosMemoryCheckerArgs); 
				//felicia, 2012/09/26
				AosBuff oldbuff(eBuffInitSize AosMemoryCheckerArgs); 
				AosBuff newbuff(eBuffInitSize AosMemoryCheckerArgs); 
				obj->setContentsToBuff(oldbuff);
				setContentsToBuff(newbuff);
				bool rslt = modifyRecord(crt_offset,kk, oldbuff, newbuff, buff, pos, file, bucket_size, rdata.getPtr());
				aos_assert_rl(rslt, mLock, false);
				mLock->unlock();
				return true;
			}
		}

		crt_offset = buff.getU64(0);
		if (crt_offset == 0) 
		{
			// Did not find the entry
			mLock->unlock();
			OmnAlarm << "To modify an entry but not found: " << crt_offset << enderr;
			return false;
		}

		// It is continued at 'continue_field'.
		bool rslt = readToBuff(buff, isEmpty, file, crt_offset, bucket_size, rdata.getPtr());
		aos_assert_rl(rslt, mLock, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


/*
bool
AosHashedObj::readBucketRecord(
		const OmnFilePtr &file, 
		const u64 &offset, 
		AosBuff &buff,
		const int bucket_size) 
{
	// This function reads the bucket record pointed to at 'offset'. 
	// If the bucket was not initialized yet, it will initialize it.
	// Bucket format is:
	// 		bucket length 	(u32)
	// 		front_poison	(u32)
	// 		data			(variable)
	// 		data
	// 		...
	// 		empty string	(4 bytes)
	// 		continue_field	(u64)
	//		end_poison		(u32)
	aos_assert_r(file, false);
	bool rslt = file->readToBuff(offset, buff.buffLen(), buff.data());
	if (rslt)
	{
		u32 bucket_len = buff.getU32(0);
		if (bucket_len > 0 && bucket_len <= (u32)bucket_size)
		{
			// It looks like it is a good record, but still we need to check the 
			// poisons.
			u32 front_poison = buff.getU32(0);
			if (front_poison == eFrontPoison)
			{
				// The front poison is good. Check the end poison and length
				u32 end_len = file->readBinaryU32(offset + bucket_len - sizeof(u32), 0);
				aos_assert_r(end_len == bucket_len, false);

				u32 end_poison = file->readBinaryU32(offset + bucket_len 
						- sizeof(u32) - sizeof(u32), 0);
				aos_assert_r(end_poison == eEndPoison, false);

				// It is a good one. 
				buff.backU32();	
				buff.backU32();	
				return true;
			}
		}
	}

	// The bucket is not good yet. Need to initialize the bucket.
	rslt = createNewBucketRecord(file, offset, bucket_size, buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosHashedObj::createNewBucketRecord(
		const OmnFilePtr &file, 
		const u64 &offset, 
		AosBuff &buff) 
{
	// This function creates a new bucket record at the position 'offset'.
	// 		length			(u32)
	// 		front-poison	(u32)
	// 		contents		(variable)
	// 		empty string	(sizeof(int))
	// 		continue_field	(u64)
	// 		end-poison		(u32)
	// 		length			(u32)
	//
	const int len = eLengthSize + ePoisonSize + eEmptyStrSize + 
		eContinueSize + ePoisonSize + eLengthSize;
	buff.reset();
	buff.setU32(len);
	buff.setU32(eFrontPoison);
	buff.setOmnStr("");
	buff.setU64(0);
	buff.setU32(eEndPoison);
	buff.setU32(len);
	
	bool rslt = file->put(offset, buff.data(), buff.dataLen(), true);
	aos_assert_r(rslt, false);

	return true;
}
*/


bool 
AosHashedObj::modifyRecord(
		const u64 &crt_offset,
		const OmnString &key,
		AosBuff &oldbuff, 
		AosBuff &newbuff, 
		AosBuff &buff, 
		const int pos, 
	    //const OmnFilePtr &file,		// Ketty 2012/11/29
		const AosReliableFilePtr &file, 
		const int bucket_size,
		const AosRundataPtr &rdata)
{
	// This function modifies the bucket 'buff' at the position 'pos'. 
	// The old contents is 'oldbuff', the new contents is 'newbuff'. 
	// 1. If the bucket is not big enough for the modification, it needs
	//    to remove the old contents, allocate a new bucket, chain 
	//    the existing bucket to the new bucket, and add the new contents
	//    to the new bucket. 
	// 2. If the bucket is big enough, just replace the contents. 
	u32 old_len = oldbuff.dataLen();
	u32 new_len = newbuff.dataLen();
	u32 delta = new_len - old_len;
	// Determine whether there is enough space
	u32 space_left = bucket_size - buff.dataLen();
	if (space_left >= delta)
	{
		// There is enough space. 
		if (delta > 0 && (buff.dataLen() + (int)delta >= buff.buffLen()))
		{
			int desired_size = delta = buff.dataLen();
			bool rslt = buff.expandMemory1(buff.getNewSize(desired_size));
			aos_assert_r(rslt, false);
		}

		char *data = buff.data();
		memmove(&data[pos+new_len], &data[pos + old_len], delta);
		memcpy(&data[pos], newbuff.data(), new_len);
		bool rslt = file->put(crt_offset, buff.data(), bucket_size, true, rdata.getPtr());
		return rslt;
	}	

	// There is not enough space. Erase the old contents from the current
	// buff, allocate a new buff, and chain them.
	// Note that we need to erase the key, too.
	int key_len = AosBuff::getStrSize(key.length());
	aos_assert_r(pos - key_len > 0, false);
	int new_pos = pos - key_len;

	// Adjust the length fields
	buff.reset();
	u32 old_buff_len = buff.getU32(0);
	aos_assert_r(old_buff_len >= old_len + key_len, false);
	u32 new_buff_len = old_buff_len - (old_len + key_len);
	buff.reset();
	buff.setU32(new_buff_len);
	
	buff.setCrtIdx(old_buff_len - 4);
	buff.setU32(new_buff_len);

	// Allocate a new bucket
	u64 new_offset = crt_offset;
	AosBuffPtr new_buff = allocateNewBucket(file, new_offset, old_buff_len, bucket_size, rdata.getPtr());

	buff.setCrtIdx(old_buff_len);
	buff.backU32();			// Back off length
	buff.backU32();			// Back end poison 
	buff.backU64();			// Back continuation field
	buff.setU64(new_offset);

	// Move the memory
	char *data = buff.data();
	memmove(&data[new_pos], &data[pos+old_len], old_len + key_len);
	file->put(crt_offset, buff.data(), buff.dataLen(), false, rdata.getPtr());

	new_buff->expandMemory1(new_buff->getNewSize(new_len));
	char *mem = new_buff->data();
	u32 pos1 = 8;
	int move_len = new_buff->dataLen() - 8;
	memmove(&mem[pos1+new_len], &mem[pos1], move_len);
	memcpy(&mem[pos1], newbuff.data(), new_len);
	file->put(new_offset, mem, new_buff->dataLen() + new_len, true, rdata.getPtr());

	return true;
}

