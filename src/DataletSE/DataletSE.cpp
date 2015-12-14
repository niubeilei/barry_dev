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
// 2014/11/07 Created by Chen Ding. 
////////////////////////////////////////////////////////////////////////////
#include "DataletSE/DataletSE.h"



AosDataletSE::AosDataletSE()
{
}


AosDataletSE::~AosDataletSE()
{
}


bool 
AosDataletSE::init()
{
	return true;
}


bool 
AosDataletSE::clear(AosRundata *rdata)
{
	OmnNotImplementedYet;
}


bool 
AosDataletSE::saveDatalet(
		AosRundata *rdata, 
		const char * const blob, 
		const int blob_len,
		const u64 snapshot, 
		const u64 transid,
		const u64 docid) 
{
	// It composes an entry and appends the entry to the active raw file. 
	// It then updates the HeaderCache. 
	//
	// If NVRAM is used, the current raw file is in NVRAM. 
	
	// --- Check whether the file is too big. 
	if (blobTooBig(blob_len))
	{
		AosLogError(rdata, true, AosErrmsgId::eDataTooBig)
			<< AosFieldName::eMaxSize << mMaxDataletSize 
			<< AosFieldName::eActualSize << blob_len << enderr;
		return false;
	}

	aos_assert_rr(mActiveRawFile, rdata, false);

	// --- Check whether it is a 'huge blob'. Huge blobs are saved directly
	//     in separate raw files.
	if (isHugeDatalet(blob_length))
	{
		return saveHugeDatalet(rdata, blob, snapshot);
	}

	// --- Create the entry
	mLockRaw->lock();
	u32 offset = mActiveRawFileSize;
	AosBuffPtr entry = constructEntry(rdata, blob, blob_len, snapshot, transid, docid, 
			encryption_id, compression_id);
	if (!entry)
	{
		AosLogInternalError(rdata);
		return false;
	}

	// --- Append the entry to the active raw file.
	bool rslt = mActiveRawFile->append(rdata, entry->data(), entry->dataLength());
	if (!rslt)
	{
		mLockRaw->unlock();
		AosLogInternalError(rdata);
		return false;
	}

	// 2. If the entry is too big, create a separate file for it.
	// Note that there is a max 
	// 3. Update HeaderCache
	mHeaderCache->updateHeader(rdata, docid, snapshot, mActiveRawfid, offset);

	// 4. Check whether the active file is full. If yes, create a new one.
	mActiveRawFileSize += entry->dataLength();
	if (isActiveFileTooBig())
	{
		bool rslt = createNewActiveRawFileLocked(rdata);
		if (!rslt)
		{
			mLockRaw->unlock();
			AosLogInternalError(rdata);
			return false;
		}
	}

	if (mChangeLogEnabled) addToChangeLog(rdata, entry);
	mLockRaw->unlock();
	return true;
}


AosBuffPtr
AosDataletSE::constructEntry(
		AosRundata *rdata, 
		const char *blob, 
		const int blob_len,
		const u64 snapshot, 
		const u64 trans_id,
		const u64 docid,
		const u8 encryption_id,
		const u8 compression_id)
{
	// Entry format is:
	// 	Length			(u32) 
	// 	Entry Type		(u8) 
	// 	checksum ID		(u8) 
	// 	Encryption ID	(u8) 
	// 	Compression	ID	(u8) 
	// 	Transid			(u64) 
	// 	Docid			(u64) 
	// 	Snapshot		(u64) 
	// 	Header			(eHeaderLength) 
	// 	blob length		(int)
	// 	blob body 		(variable) 
	// 	checksum		(variable)
	// 	Length			(u32)
	//
	// Compression and encryption are on the body only.

	// Header format is:
	// 	Status			(u8) 
	// 	Rawfid			(u64) 
	// 	Offset			(u32) 
	// 	Length			(int)     
	// It is fixed length (eHeaderSize). 

	// --- Construct the header
	char header_buff[eHeaderSize];
	int idx = 0;
	header_buff[idx] = eHeaderStatusActive;			idx++;
	*(u64*)&header_buff[idx] = mActiveRawfid;		idx += sizeof(u64);
	*(u32*)&header_buff[idx] = mActiveRawFileSize;	idx += sizeof(u32);
	*(int*)&header_buff[idx] = body_length;			idx += sizeof(int);
	aos_assert_rr(idx == eHeaderSize, rdata, false);

	const char * const blob_body = blob;
	AosBuffPtr buff;
	int len = blob_len;
	if (needEncrypt(encryption_id))
	{
		buff = AosEncrypt(rdata, encryption_id, mEncryptionKey, blob);
		blob_body = buff->data();
		len = buff->dataLen();
	}

	if (needCompress(compression_id))
	{
		buff = AosCompress(rdata, compression_id, blob_body, len);
		blob_body = buff->data();
		len = buff->dataLen();
	}

	int checksum_len = -1;
	switch (mChecksumType)
	{
	case eChecksumTypeSHA1:
		 checksum_len = 20;
		 break;

	case eChecksumTypeMD5:
		 checksum_len = 16;
		 break;

	case eChecksumTypeSimple:
	default:
		 // It calculates the sum (u64) of all the values
		 checksum_len = sizeof(u64);
		 break;
	}

	int entry_length = 
			sizeof(u32) +		// Length
			1 +					// Entry type
			1 +					// Encryption ID
			1 +					// Compressoin ID
			sizeof(u64) +		// Transid
			sizeof(u64) + 		// Docid
			sizeof(u64) +		// snapshot
			eHeaderLength +		// Header
			sizeof(blob_len) +	// Datalet length
			len +				// Body actual length	
			checksum_len +		// Checksum length
			sizeof(u32);		// Length


	switch (mChecksumType)
	{
	case eChecksumTypeSimple:
	default:
		 // It calculates the sum (u64) of all the values
		 checksum = 0;
		 checksum += entry_length + eEntryTypeData + encryption_id + compression_id
			+ transid + docid + snapshot + eHeaderLength + blob_len;
		 for (u32 i=0; i<blob_len; i++)
		 {
			checksum += blob_body[i];
		 }
		 break;
	}

	
	// --- Construct the body
	AosBuffPtr buff = OmnNew AosBuff(buff_length+100);
	buff->setU32(entry_length);
	buff->setU8(eEntryTypeData);
	buff->setU8(encryption_id);
	buff->setU8(compression_id);
	buff->setU64(transid);
	buff->setU64(docid);
	buff->setU64(snapshot);
	buff->setBuff(header_buff, eHeaderSize);
	buff->setInt(blob_len);
	buff->setBuff(blob_body, blob_len);
	buff->setU32(entry_length);

	return buff;
}


bool
AosDataletSE::isActiveFileFull()
{

}


bool 
AosDataletSE::deleteDoc(
		AosRundata *rdata, 
		const u64 docid, 
		const u64 trans_id,
		const u64 snapshot)
{
	// --- Construct an entry
	// Entry format is:
	// 	Length			(u32) 
	// 	Entry Type		(u8) 
	// 	Transid			(u64) 
	// 	Docid			(u64) 
	// 	Snapshot		(u64) 
	// 	Length			(u32)

	char buff[eDeleteEntrySize];
	*(u32*)buff = eDeleteEntrySize;			idx = sizeof(u32);
	buff[idx] = eEntryTypeDelete;			idx++;
	*(u64*)&buff[idx] = transid;			idx += sizeof(u64);
	*(u64*)&buff[idx] = docid;				idx += sizeof(u64);
	*(u64*)&buff[idx] = snapshot;			idx += sizeof(u64);
	*(u32*)&buff[idx] = eDeleteEntrySize;	idx += sizeof(u64);

	// --- Append the entry to the active raw file.
	bool rslt = mActiveRawFile->append(rdata, entry->data(), entry->dataLength());
	if (!rslt)
	{
		mLockRaw->unlock();
		AosLogInternalError(rdata);
		return false;
	}

	// 2. If the entry is too big, create a separate file for it.
	// Note that there is a max 
	// 3. Update HeaderCache
	mHeaderCache->deleteHeader(rdata, docid, snapshot);

	// 4. Check whether the active file is full. If yes, create a new one.
	mActiveRawFileSize += entry->dataLength();
	if (isActiveFileTooBig())
	{
		bool rslt = createNewActiveRawFileLocked(rdata);
		if (!rslt)
		{
			mLockRaw->unlock();
			AosLogInternalError(rdata);
			return false;
		}
	}

	if (mChangeLogEnabled) addToChangeLog(rdata, entry);
	mLockRaw->unlock();
	return true;
}


AosBuffPtr
AosDataletSE::readDoc(
		AosRundata *rdata, 
		const u64 docid, 
		const u64 snapshot)
{
	// Header format is:
	// 	Status			(u8) 
	// 	Rawfid			(u64) 
	// 	Offset			(u32) 
	// 	Length			(int)     
	mLockRaw->lock();
	u64 rawfid;
	u32 offset;
	int length;
	bool rslt = mHeaderCache->readHeader(rdata, docid, snapshot, rawfid, offset, length);
	if (!rslt)
	{
		// The blob does not exist in DataletSE
		mLockRaw->unlock();
		AosLogError(rdata, true, AosErrmsgId::eDataletDoesNotExist)
			<< AosFieldName::eDocid << docid
			<< AosFieldName::eSnapshot << snapshot << enderr;
		return 0;
	}
	mLockRaw->unlock();

	AosBuffPtr buff;
	if (!mBodyCache->readFile(rdata, rawfid, offset, length, buff))
	{
		// Failed reading the file. 
		return 0;
	}
	aos_assert_rr(buff, rdata, 0);

	// Verify Checksum
	if (!verifyChecksum(rdata, buff)) return 0;

	// 	Entry Length	(u32) 
	// 	Entry Type		(u8) 
	// 	Encryption ID	(u8) 
	// 	Compression	ID	(u8) 
	// 	Transid			(u64) 
	// 	Docid			(u64) 
	// 	Snapshot		(u64) 
	// 	Header			(eHeaderLength) 
	// 	blob length		(int)
	// 	blob body 		(variable) 
	// 	checksum		(variable)
	// 	Entry Length	(u32)
	
	u32 entry_length = buff->getU32(0);
	u8 entry_type = buff->getU8(eEntryTypeInvalid);
	u8 encryption_id = buff->getU8(0);
	u8 compression_id = buff->getU8(0);
	u64 transid = buff->getU64(0);
	u64 docid = buff->getU64(0);
	u64 snapshot = buff->getU64(0);
	buff->skip(eHeaderSize);
	int blob_len = buff->getInt(-1);
	
	AosBuffPtr body_buff(blob_len);
	if (!buff->getBuff(body_buff->data(), body_buff->dataLen()))
	{
		AosLogInternalError(rdata);
		return 0;
	}

	if (needCompression(compression_id))
	{
		body_buff = AosDecompress(rdata, compression_id, body_buff->data(), body_buff->dataLen());
	}

	if (needEncryption(encryption_id))
	{
		body_buff = AosEncrypt(rdata, encryption_id, mEncryptionKey, body_buff);
	}

	if (body_buff->dataLen() != blob_len)
	{
		AosSetError(rdata, true, AosErrmsgId::eSizeMismatch)
			<< AosFieldName::eExpectedLength << blob_len
			<< AosFieldName::eActualLength << body_buff->dataLen()
			<< AosFieldName::eDocid << docid
			<< AosFieldName::eSnapshot << snapshot << enderr;
		return 0;
	}

	return body_buff;
}

