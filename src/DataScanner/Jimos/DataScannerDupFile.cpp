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
// This data scanner has only one file but the file is distributed
// over the entire cluster with exactly the same name.
//
// Modification History:
// 2013/11/21: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/Jimos/DataScannerDupFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include "Util/DiskStat.h"


AosDataScannerDupFile::AosDataScannerDupFile()
:
AosDataScanner(),
mStartPos(0),
mFileLength(0),
mReadBlockSize(0)
{
}


AosDataScannerDupFile::AosDataScannerDupFile(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
:
AosDataScanner(),
mStartPos(0),
mFileLength(0),
mReadBlockSize(0)
{
	if (!initFile(rdata, def))
	{
		OmnThrowException("missing_data_file_def");
		return;
	}
}


AosDataScannerDupFile::AosDataScannerDupFile(
		const AosRundataPtr &rdata, 
		const OmnString &objid)
{
	if (!initFile(rdata, objid))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataScannerDupFile::~AosDataScannerDupFile()
{
}


bool
AosDataScannerDupFile::initFile(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	aos_assert_rr(doc, rdata, false);
	return initFile(rdata, doc);
}


bool
AosDataScannerDupFile::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	//	<scanner type="file" ...>
	//		<metadata ...
	//			zky_physicalid="xxxx"
	//			zky_readsize="xxxx"
	//			zky_record_len="xxxx"
	//			zky_max_records="xxxx"
	//			zky_max_tasks="xxxx"
	//			zky_fullname="../data/voice.txt"
	//			zky_startpos="0"
	//			zky_length="-1">
	//		</metadata>
	//		path
	// </scanner>
	aos_assert_rr(def, rdata, false);

	mMetadata = def->getFirstChild("metadata");
	if (!mMetadata)
	{
		AosSetErrorUser(rdata, "missing_file_def") << def->toString() << enderr;
		return false;
	}
	
	mMetadata = mMetadata->clone(AosMemoryCheckerArgsBegin);
	mPhysicalid = mMetadata->getAttrInt("zky_physicalid", -1);
	mCharset = mMetadata->getAttrStr("charaset");
	if (!AosIsValidPhysicalIdNorm(mPhysicalid))
	{
		AosSetErrorUser(rdata, "invalid_physical_id") << def->toString() << enderr;
		return false;
	}

	mFileName = def->getNodeText();
	if (mFileName == "")
	{
		AosSetErrorUser(rdata, "missing_file_path") << def->toString() << enderr;
		return false;
	}

	AosDiskStat disk_stat;
	bool rslt = AosGetFileLength(rdata, mFileName, mPhysicalid, mFileLength, disk_stat);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


int64_t
AosDataScannerDupFile::getTotalSize() const
{
	return mFileLength;
}


AosDataScannerObjPtr
AosDataScannerDupFile::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataScannerDupFile(rdata, conf);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDataScannerDupFile::getNextBlock(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	if (!info)
	{
		info = OmnNew AosBuffData();
	}
	aos_assert_r(info, false);

	AosBuffPtr buff;
	int64_t start_pos = mStartPos;
	AosDiskStat disk_stat;
	bool rslt = getNextBlock(rdata, buff, disk_stat);
	aos_assert_r(rslt, false);
	info->setBuff(buff);
	info->setDiskStat(disk_stat);
	int64_t data_len = 0;
	if (buff) data_len = buff->dataLen();

	AosXmlTagPtr xml = mMetadata->clone(AosMemoryCheckerArgsBegin);
	xml->setAttr(AOSTAG_SOURCE_FNAME, mFileName);
	xml->setAttr(AOSTAG_SOURCE_LENGTH, mFileLength);
	xml->setAttr("start_pos", start_pos);
	xml->setAttr("data_len", data_len);

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
AosDataScannerDupFile::getNextBlock(
		const AosRundataPtr &rdata,
		AosBuffPtr &buff, 
		AosDiskStat &disk_stat)
{
	// It reads in the next block. The scanner has a number of files. 
	// 'mStartPos' is the current position from which the next read starts reading
	// 'mFileLength' is the size of the current file. 
	// 'mReadBlockSize' is the read block size.
	//
	// If the current file was completely read, it moves on to the next file.
	// If no more file to read, it returns null. 
	// If the current file has some contents to read, it reads either 
	// the remaining contents up to 'mReadBlockSize'. 
	
	aos_assert_rr(mStartPos >= 0, rdata, false);
	aos_assert_rr(mFileLength >= 0, rdata, false);
	aos_assert_rr(mReadBlockSize > 0, rdata, false);
	buff = 0;
	mLock->lock();
	if (mStartPos >= mFileLength) 
	{
		mLock->unlock();
		return true;
	}

	int64_t bytes_to_read = mFileLength - mStartPos;
	if (bytes_to_read > mReadBlockSize)
	{
		bytes_to_read = mReadBlockSize;
	}

	u64 tt1 = OmnGetTimestamp();
	bool rslt = AosNetFileCltObj::readFileToBuffStatic(
		mFileName, mPhysicalid, mStartPos, bytes_to_read, buff, disk_stat, rdata);
	u64 tt2 = OmnGetTimestamp();
	disk_stat.total_opt_time = tt2-tt1;

	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(buff, mLock, false);
	u32 bytes_read = buff->dataLen();

	OmnScreen << "\n========================================" 
			  << "\n== file name: " << mFileName
			  << "\n== file length: " << AosNumber2String(mFileLength)
		  	  << "\n== total read block: " << AosNumber2String(mFileLength)
		  	  << "\n== already read length: " << AosNumber2String(bytes_read)
			  << "\n== current read bytes: " << AosNumber2String(bytes_read)
			  << "\n========================================\n" << endl;

	mStartPos += bytes_read;
	mLock->unlock();

	if (mCharset != CodeConvertion::mDefaultType)
	{
		int64_t len = buff->dataLen() * 2;
		AosBuffPtr newbuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
		OmnString to_type = CodeConvertion::mDefaultType;
		int64_t newlen = CodeConvertion::convert(mCharset.data(), to_type.data(),
				buff->data(), buff->dataLen(), newbuff->data(), len);
		aos_assert_r(newlen >= 0, false);
		newbuff->setDataLen(newlen);
		buff = newbuff;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosDataScannerDupFile::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	buff->setOmnStr(mMetadata->toString());
	buff->setOmnStr(mFileName);
	buff->setI64(mStartPos);
	buff->setI64(mFileLength);
	buff->setInt(mPhysicalid);
	return false;
}


bool 
AosDataScannerDupFile::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	mMetadata = AosXmlParser::parse(buff->getOmnStr("") AosMemoryCheckerArgs);
	aos_assert_rr(mMetadata, rdata, false);
	mFileName = buff->getOmnStr("");
	aos_assert_rr(mFileName != "", rdata, false);
	mStartPos = buff->getInt64(0);
	mFileLength = buff->getInt64(0);
	mPhysicalid = buff->getInt(0);
	return false;
}


bool
AosDataScannerDupFile::getInstances(
		const AosRundataPtr &rdata, 
		vector<AosDataScannerObjPtr> &instances)
{
	instances = mScanners;
	return true;
}

#endif
