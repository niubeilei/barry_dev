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
// 07/03/2012 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/FileReadRunner.h"

#include "Rundata/Rundata.h"
#include "API/AosApi.h"

u32 AosFileReadRunner::smReadSize = AosFileReadRunner::eDftReadSize;
AosFileReadRunner::AosFileReadRunner(
				const AosDataScannerObjPtr &scanner,
				const AosXmlTagPtr &def,
				const int64_t &start_pos,
				const int64_t &length,
				const int64_t &filelen,
				const AosRundataPtr &rdata)
:
mScanner(scanner),
mStartPos(start_pos),
mLength(length),
mFileLen(filelen),
mRundata(rdata)
{
	bool rslt = config(def, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosFileReadRunner::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	try
	{
		aos_assert_rr(def, rdata, false);
		mFileName = def->getAttrStr("zky_fullname", "");
		mPhysicalId = def->getAttrInt("zky_physicalid", -1);
		aos_assert_rr(mPhysicalId != -1, rdata, false);
		mRecordLen = def->getAttrInt64("zky_record_length", -1);
		smReadSize = def->getAttrU32("zky_readsize", eDftReadSize);
		if (smReadSize < eMinReadSize) smReadSize = eMinReadSize;
		if(mLength == -1 || mLength + mStartPos > mFileLen)
		{
			mLength = mFileLen - mStartPos;
		}
		return true;
	}
	catch (...)
	{
		OmnAlarm << "Failed initializing " << enderr;
		return false;
	}
	OmnShouldNeverComeHere;
	return false;
}


bool
AosFileReadRunner::run()
{
	AosRundataPtr rdata = mRundata;
	aos_assert_rr(mLength > 0, rdata, false);
	aos_assert_rr(smReadSize > 0, rdata, false);
	aos_assert_rr(mScanner, rdata, false);

	int64_t seekPos = mStartPos;
	u32 buff_size = smReadSize;
	if (smReadSize > mLength)
	{
		buff_size = mLength;
	}
	AosBuffPtr buff = OmnNew AosBuff(buff_size, 0 AosMemoryCheckerArgs);
	u32 bytes_to_read = buff_size;
	int64_t already_readlen = 0;
	int64_t remaining_bytes = 0;
	bool rslt;
	while(already_readlen < mLength)
	{
		AosBuffPtr filebuff;
		rslt = AosNetFileCltObj::readFileToBuffStatic(
			mFileName, mPhysicalId, seekPos, bytes_to_read, filebuff, rdata);
		aos_assert_rr(rslt, rdata, false);
		u32 bytes_read = filebuff->dataLen();
		if (bytes_read <= 0)
		{
			break;
		}
OmnScreen << "********************" 
		  << "\r\n    buff length: " << filebuff->buffLen()
		  << "\r\n        seekPos: " << seekPos 
		  << "\r\n     bytes read: " << bytes_read 
		  << "\r\n  bytes to read: " << bytes_to_read 
		  << "\r\nremaining bytes: " << remaining_bytes << endl;
		//test-start
		aos_assert_r(recoverFile(filebuff, bytes_read, seekPos, rdata), false);
		//test-end
		buff->setCrtIdx(remaining_bytes);
		buff->setBuff(filebuff);	
		//add to datacacher
		mScanner->dataRead(buff, rdata);

		seekPos += bytes_to_read;
		already_readlen += bytes_to_read;
		if (already_readlen >= mLength) break;
		remaining_bytes = (bytes_read+remaining_bytes) % mRecordLen;
		if (remaining_bytes > 0)
		{
			memmove(buff->data(), &(buff->data()[buff_size - remaining_bytes]), remaining_bytes);
		}

		int64_t remain_bytes_in_file = mLength - already_readlen;
		int64_t remain_bytes_in_buff = buff_size - remaining_bytes;
		if (remain_bytes_in_file <= remain_bytes_in_buff)
		{
			bytes_to_read = remain_bytes_in_file;
		}
		else
		{
			bytes_to_read = remain_bytes_in_buff;
		}
	}
	return true;
}

bool
AosFileReadRunner::recoverFile(
		const AosBuffPtr &buff,
		const u32 bytes_read,
		const int64_t &seekPos,
		const AosRundataPtr &rdata)
{
	OmnString fname = "/home/jozhi/FileScannerTestData/xxxx";
	return 	AosNetFileCltObj::writeBuffToFileStatic(
		fname, 1, seekPos, buff, rdata);
}


bool
AosFileReadRunner::procFinished()
{
	mScanner->threadFinished();
	return true;
}

#endif
