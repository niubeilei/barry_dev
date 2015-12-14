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
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/FileScanner.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/UtUtil.h"
#include "XmlUtil/SeXmlParser.h"

AosFileScanner::AosFileScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_FILE, AosDataScannerType::eFile, flag),
mFileName(""),
mStart(-1),
mLength(-1),
mPhysicalid(-1),
mRecordLen(0),
mFileLen(-1),
mReadLength(0),
mCrtReadIdx(0),
mFileReadSize(eMinBuffSize),
mLock(OmnNew OmnMutex()),
mIgnoreHead(false)
{
	mMaxTasks = AosGetNumCpuCores();
}


AosFileScanner::AosFileScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_FILE, AosDataScannerType::eFile, false),
mFileName(""),
mStart(-1),
mLength(-1),
mPhysicalid(-1),
mRecordLen(0),
mFileLen(-1),
mReadLength(0),
mCrtReadIdx(0),
mFileReadSize(eMinBuffSize),
mLock(OmnNew OmnMutex()),
mIgnoreHead(false)
{
	mMaxTasks = AosGetNumCpuCores();

	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosFileScanner::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//<scanner type="file">
	//	<readfile>
	//		<entry
	//			zky_physicalid="xxxx"
	//			zky_readsize="xxxx"
	//			zky_record_len="xxxx"
	//			zky_max_records="xxxx"
	//			zky_max_tasks="xxxx"
	//			zky_fullname="../data/voice.txt"
	//			zky_startpos="0"
	//			zky_length="-1">
	//		</entry>
	//	</readfile>
	//	<writefile
	//			zky_fullname="../data/voice.txt"
	//			zky_physicalid="xxxx"
	//			zky_startpos="0"
	//			zky_length="-1">
	//	</writefile>
	//</scanner>
	if (def)
	{
		mReadFileInfo = def->getFirstChild("readfile");
		mWriteFileInfo = def->getFirstChild("writefile");
	}
	return true;
}


AosFileScanner::~AosFileScanner()
{
}


AosJimoPtr
AosFileScanner::cloneJimo() const
{
	try
	{
		return OmnNew AosFileScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr
AosFileScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosFileScanner(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


bool
AosFileScanner::getNextBlock(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	if (!info)
	{
		info = OmnNew AosBuffData();
	}
	// JACKIE-HADOOP
	int64_t offset = mStart;

	aos_assert_r(info, false);
	AosBuffPtr buff;
	bool rslt = getNextBlock(buff, rdata);
	aos_assert_r(rslt, false);
	info->setBuff(buff);

	// JACKIE-HADOOP
	info->setOffset(offset);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse("<metaData/>", "" AosMemoryCheckerArgs);
	xml->setAttr(AOSTAG_SOURCE_FNAME, mFileName);
	xml->setAttr(AOSTAG_SOURCE_LENGTH, mLength);
	int idx = mFileName.indexOf(0, '/', true);
	aos_assert_r(idx >= 0, false);
	OmnString name = mFileName.substr(idx + 1);
	xml->setAttr(AOSTAG_SOURCE_NAME, name);
	OmnString path = mFileName.substr(0, idx - 1);
	xml->setAttr(AOSTAG_SOURCE_PATH, path);
	info->addMetadata(xml);
	return true;
}


bool
AosFileScanner::getNextBlock(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// It reads in the next block.
	bool rslt = false;
	if (mRowDelimiter == "")
	{
		rslt = readFixedLengthToBuff(buff, rdata);

	}
	else
	{
		rslt = readVarLengthToBuff(buff, rdata);
	}
	if (!buff || buff->dataLen() <= 0)
	{
		return true;
	}
	if (mCharacterType != CodeConvertion::mDefaultType)
	{
		int64_t len = buff->dataLen() * 2;
		AosBuffPtr newbuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
		OmnString to_type = CodeConvertion::mDefaultType;
		int64_t newlen = CodeConvertion::convert(mCharacterType.data(), to_type.data(),
				buff->data(), buff->dataLen(), newbuff->data(), len);
		aos_assert_r(newlen >= 0, false);
		newbuff->setDataLen(newlen);
		buff = newbuff;
	}
	aos_assert_r(rslt, false);
	return true;
}

bool
AosFileScanner::readFixedLengthToBuff(
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(mStart >= 0, mLock, false);
	aos_assert_rl(mLength >= 0, mLock, false);
	if (mReadLength >= mLength)
	{
		mLock->unlock();
		return true;
	}

	int64_t bytes_to_read = eMinBuffSize - eMinBuffSize % mRecordLen;
	if (bytes_to_read > mRecordLen * eMinRecordSize)
	{
		bytes_to_read = mRecordLen * eMinRecordSize;
	}
	if (mLength - mReadLength < bytes_to_read)
	{
		bytes_to_read = mLength - mReadLength;
	}

	AosDiskStat disk_stat;
	// JACKIE-HADOOP
	bool rslt;
	if (mIsHadoop)
	{
		rslt = AosReadHadoopFile(rdata, buff, mFileName, mStart, bytes_to_read);
	}
	else
	{
		rslt = AosReadDataFile(rdata.getPtr(), buff, mPhysicalid, mFileName, 
			mStart, bytes_to_read, disk_stat);
	}
	aos_assert_rl(rslt, mLock, false);
	if (disk_stat.serverIsDown())
	{
OmnScreen << "=====================disk error1111111111111111 : " << endl;
		mDiskError = true;
		mLock->unlock();
		return true;
	}

	u32 bytes_read = buff->dataLen();

	OmnScreen << "\n========================================" 
			  << "\n== file name: " << mFileName
			  << "\n== file length: " << AosNumber2String(mFileLen)
		  	  << "\n== total read block: " << AosNumber2String(mLength)
		  	  << "\n== already read length: " << AosNumber2String(mReadLength)
			  << "\n== current read bytes: " << AosNumber2String(bytes_read)
			  << "\n========================================\n" << endl;

	mStart += bytes_read;
	mReadLength += bytes_read;
	mLock->unlock();
	return true;
}

bool
AosFileScanner::readVarLengthToBuff(
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(mStart >=0, mLock, false);
	if (mReadLength >= mLength)
	{
		mLock->unlock();
		return true;
	}
	int64_t bytes_to_read = eMinBuffSize;
	if (mLength - mReadLength < bytes_to_read)
	{
		bytes_to_read = mLength - mReadLength;
	}
	AosDiskStat disk_stat;
	// JACKIE-HADOOP
	bool rslt;
	if (mIsHadoop)
	{
		rslt = AosReadHadoopFile(rdata, buff, mFileName, mStart, bytes_to_read);
	}
	else
	{
		rslt = AosReadDataFile(rdata.getPtr(), buff, mPhysicalid, mFileName, 
			mStart, bytes_to_read, disk_stat);
	}
	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(mRowDelimiter != "", mLock, false);
	char* data = buff->data();
	char* last = strrstr(data, buff->dataLen(), mRowDelimiter.data());
	if(last)
	{
		char* end = last + mRowDelimiter.length();
		u32 real_len = end - data;
		if(real_len > bytes_to_read)
		{
			OmnAlarm << "read len error" <<enderr;
			real_len = bytes_to_read;
		}
		buff->setDataLen(real_len);
	}
	u32 bytes_read = buff->dataLen();
	if (mIgnoreHead && mStart == 0)
	{
		char* head = strstr(data, mRowDelimiter.data());
		if (head)
		{
			char* real_start = head + mRowDelimiter.length();
			char* real_end = buff->data() + bytes_read;
			u32 real_len = real_end - real_start;
			AosBuffPtr newbuff = OmnNew AosBuff(real_len AosMemoryCheckerArgs);
			newbuff->setBuff(real_start, real_len);
			newbuff->setDataLen(real_len);
			//buff->setBuff(real_start, real_len);
			//buff->setDataLen(real_len);
			buff = newbuff;
		}
	}

	mStart += bytes_read;
	mReadLength += bytes_read;
	/*
	OmnScreen << "\n========================================" 
			  << "\n== file name: " << mFileName
			  << "\n== file length: " << AosNumber2String(mLength)
		  	  << "\n== already read length: " << AosNumber2String(mReadLength)
			  << "\n== current read bytes: " << AosNumber2String(bytes_read)
			  << "\n========================================\n" << endl;
	*/
	mLock->unlock();
	return true;
}


bool
AosFileScanner::getNextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	// Chen Ding, 2013/11/29
	// if (mDelimiter != "") return getNextRecordByDelimiter(record, rdata);
	if (mRecordLen > 0) return getNextRecordFixedLen(record, rdata);

	AosSetErrorU(rdata, "invalid_record_type") << enderr;
	return false;
}


/*
bool
AosFileScanner::getNextRecordByDelimiter(
		AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	// This function reads in a record. Records are defined by delimiters. 
	// This means that it reads from the current position to the next 
	// delimiter or to the end of the file. The current position is stored
	// in 'mStart'. The total length read so far is stored in 'mReadLength'.
	// The function uses an in-memory cache to read a block when needed. 
	// After words, all reading is in memory. When the contents are used 
	// up, it reads in the next block until the entire file finishes.
	int crt_idx = mCrtReadIdx;
	if (!mBuff) 
	{
		mStart = 0;
		mReadLength = 0;
		mBuff = OmnNew AosBuff(mFileReadSize+100 AosMemoryCheckerArgs);
		int bytes_to_read = mFileReadSize;
		AosDiskStat disk_stat;
		bool rslt = AosReadDataFile(rdata, mBuff, mPhysicalid, mFileName, 
				mStart, bytes_to_read, disk_stat);
		aos_assert_rr(rslt, rdata, false);
		if (bytes_to_read <= 0)
		{
			// No data in the file.
			mBuff->setDataLen(0);
			return true;
		}

		mBuff->setDataLen(bytes_to_read);
	}

	int data_len = mBuff->dataLen();
	char *data = mBuff->data();
	while (crt_idx < data_len)
	{
		// Find the end of the line. 
		const char *ptr = strstr(&data[crt_idx], mRowDelimiter.data());
		if (!ptr)
		{
			// Did not find it. Need to read the next block. Before doing
			// so, it needs to move the remaining bytes to the beginning 
			// of the buff. 
			int remaining_len = data_len - crt_idx;
			if (remaining_len >= mFileReadSize)
			{
				// This means that there are no whole record in the entire 
				// buff. This should never happen.
				AosSetErrorU(rdata, "failed_read_record") << ": " << mFileReadSize << enderr;
				return false;
			}

			memcpy(data, &data[crt_idx], remaining_len);

			int bytes_to_read = mFileReadSize - remaining_len;
			AosBuffPtr tmp_buff = OmnNew AosBuff(&data[remaining_len], 
					bytes_to_read, 0 AosMemoryCheckerArgs);
			AosDiskStat disk_stat;
			bool rslt = AosReadDataFile(rdata, tmp_buff, mPhysicalid, mFileName, 
					mStart, bytes_to_read, disk_stat);
			aos_assert_rr(rslt, rdata, false);

			if (bytes_to_read <= 0)
			{
				AosSetErrorU(rdata, "failed_reading_file") << ": " << bytes_to_read << enderr;
				return false;
			}

			mStart += bytes_to_read;
			mReadLength += bytes_to_read;
			mBuff->setDataLen(bytes_to_read + remaining_len);
			data_len = mBuff->dataLen();
			crt_idx = 0;
			continue;
		}

		record->setData(&data[crt_idx], ptr - data);
		mCrtReadIdx += ptr - data;
		return true;
	}

	OmnShouldNeverComeHere;
	return false;
}
*/


bool
AosFileScanner::getNextRecordFixedLen(
		AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosFileScanner::appendBlock(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mWriteFileInfo, rdata, false);
	int physicalid = mWriteFileInfo->getAttrInt("zky_physicalid", -1);
	OmnString fname = mWriteFileInfo->getAttrStr("zky_fullname", "");
	aos_assert_rr(fname != "", rdata, false);
	return AosNetFileCltObj::appendBuffToFileStatic(
		fname, physicalid, buff, rdata.getPtr());
}


bool 
AosFileScanner::modifyBlock(
		const int64_t &seekPos, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mWriteFileInfo, rdata, false);
	int physicalid = mWriteFileInfo->getAttrInt("zky_physicalid", -1);
	OmnString fname = mWriteFileInfo->getAttrStr("zky_fullname", "");
	aos_assert_rr(fname != "", rdata, false);
	return AosNetFileCltObj::writeBuffToFileStatic(
		fname, physicalid, seekPos, buff, rdata.getPtr());
}


bool 
AosFileScanner::createFileScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const int64_t &start,
		const int64_t &len,
		const int64_t &filelen,
		const u32 recordlen,
		const AosRundataPtr &rdata)
{
	try
	{
		AosFileScanner* scanner = OmnNew AosFileScanner(false);
		scanner->mFileName = mFileName;
		scanner->mPhysicalid = mPhysicalid;
		scanner->mStart = start;
		scanner->mLength = len;
		scanner->mFileLen = filelen;
		scanner->mRecordLen = recordlen;
		scanner->mIgnoreHead = mIgnoreHead;
		scanner->mCharacterType = mCharacterType;
		scanner->mRowDelimiter = mRowDelimiter;
		scanners.push_back(scanner);
	}
	catch(...)
	{
		OmnAlarm << "Faild to create FileScanner" << enderr;
		return false;
	}
	return true;
}

bool 
AosFileScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	buff->setOmnStr(mReadFileInfo->toString());
	buff->setOmnStr(mWriteFileInfo->toString());
	buff->setOmnStr(mFileName);
	buff->setI64(mStart);
	buff->setI64(mLength);
	buff->setInt(mPhysicalid);
	return false;
}


bool 
AosFileScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	mReadFileInfo = AosXmlParser::parse(buff->getOmnStr("") AosMemoryCheckerArgs);
	aos_assert_rr(mReadFileInfo, rdata, false);
	mWriteFileInfo = AosXmlParser::parse(buff->getOmnStr("") AosMemoryCheckerArgs);
	aos_assert_rr(mWriteFileInfo, rdata, false);
	mFileName = buff->getOmnStr("");
	aos_assert_rr(mFileName != "", rdata, false);
	mStart = buff->getI64(0);
	mLength = buff->getI64(0);
	mPhysicalid = buff->getInt(0);
	return false;
}


bool
AosFileScanner::initFileScanner(
		const OmnString &fname,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const int record_len,
		//JACKIE-HADOOP
		const bool &ishadoop,
		const AosRundataPtr &rdata)
{
	mFileName = fname;
	mPhysicalid = physical_id;
	mIgnoreHead = ignore_head;
	mCharacterType = character_type;
	mRowDelimiter = row_delimiter;
	mRecordLen = record_len;

	mStart = 0;
	AosDiskStat disk_stat;
	//JACKIE-HADOOP
	mIsHadoop = ishadoop;
	if(mIsHadoop)
	{
		mLength = AosGetHadoopFileLength(rdata, mFileName);
	}
	else
	{
		bool rslt = AosNetFileCltObj::getFileLengthStatic(
			mFileName, mPhysicalid, mLength, disk_stat, rdata.getPtr());
		if (!rslt || disk_stat.serverIsDown())
		{
			mDiskError = true;
			return true;
		}
	}
	aos_assert_r(mLength> 0, false);

	return true;
}


char*
AosFileScanner::strrstr(const char* s1, const int len, const char* s2)
{
	int len2 = 0;
	if (!(len2 = strlen(s2)))
	{
		return (char*)s1;
	}

	char* pcRet = NULL;

	s1 = s1 + (len-1);

	for (int i=len; i>0; i--,--s1)
	{
		if (*s1 == *s2 && strncmp(s1, s2, len2) == 0 )
		{
			pcRet = (char *)s1;
			break;
		}
	}
	return pcRet;
}


int
AosFileScanner::getPhysicalId() const
{
	return mPhysicalid;
}
#endif
