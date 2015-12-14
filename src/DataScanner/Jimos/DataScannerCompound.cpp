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
// This data scanner has an array of individual files. Normally these
// files reside on the same physical machine, but it is also possible
// that they reside on different machines.
//	
// Modification History:
// 2013/11/21: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/Jimos/DataScannerCompound.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDataScannerCompound::AosDataScannerCompound()
:
AosDataScanner(),
mCrtScannerIdx(0)
{
}


AosDataScannerCompound::AosDataScannerCompound(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
:
AosDataScanner(),
mCrtScannerIdx(0)
{
	if (!initFile(rdata, def))
	{
		OmnThrowException("missing_data_file_def");
		return;
	}
}


AosDataScannerCompound::AosDataScannerCompound(
		const AosRundataPtr &rdata, 
		const OmnString &objid)
:
AosDataScanner(),
mCrtScannerIdx(0)
{
	if (!initFile(rdata, objid))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataScannerCompound::~AosDataScannerCompound()
{
}


bool
AosDataScannerCompound::initFile(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	aos_assert_rr(doc, rdata, false);
	return initFile(rdata, doc);
}


bool
AosDataScannerCompound::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	//	<scanner type="file" ...>
	//		<metadata .../>
	//		<files>
	//			<file .../>
	//			<file .../>
	//			...
	//		</files>
	// </scanner>
	aos_assert_rr(def, rdata, false);

	mMetadata = def->getFirstChild("metadata");
	if (mMetadata)
	{
		mMetadata = mMetadata->clone(AosMemoryCheckerArgsBegin);
	}
	else
	{
		mMetadata = createDefaultMetadata(rdata, def);
	}
	mMetadata->setAttr("instance_id", mInstanceId);

	AosXmlTagPtr files_tag = def->getFirstChild("files");
	if (!files_tag)
	{
		AosSetErrorUser(rdata, "missing_files") << def->toString() << enderr;
		return false;
	}

	AosXmlTagPtr tag = files_tag->getFirstChild();
	while (tag)
	{
		AosDataScannerObjPtr scanner = AosCreateDataScanner(rdata, tag, 0);
		aos_assert_rr(scanner, rdata, false);
		mScanners.push_back(scanner);
		tag = files_tag->getNextChild();
		aos_assert_rr(mScanners.size() < eMaxMemberScanners, rdata, false);
	}

	aos_assert_rr(mScanners.size() > 0, rdata, false);

	mCrtScannerIdx = 0;
	return true;
}


AosXmlTagPtr
AosDataScannerCompound::createDefaultMetadata(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	OmnString ss;
	ss << "<multi_scanners "
		<< "scanner_type=\"multi_scanners\""
		<< "name=\"MultiScanners\"/>";
	return AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
}


int64_t
AosDataScannerCompound::getTotalSize() const
{
	// The total length it returns is the one being calculated when 
	// this class was created. If the file sizes change since then, 
	// they are not reflected here.
	int64_t len = 0;
	for (u32 i=0; i<mScanners.size(); i++)
	{
		len += mScanners[i]->getTotalSize();
	}
	return len;
}


AosJimoPtr
AosDataScannerCompound::cloneJimo() const 
{
	try
	{
		return OmnNew AosDataScannerCompound(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr
AosDataScannerCompound::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataScannerCompound(rdata, conf);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


bool
AosDataScannerCompound::getNextBlock(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mCrtScannerIdx >= 0, rdata, false);
	mLock->lock();
	while ((u32)mCrtScannerIdx < mScanners.size())
	{
		bool rslt = mScanners[mCrtScannerIdx]->getNextBlock(info, rdata);
		aos_assert_rl(rslt, mLock, false);
		if (info && info->getDataLen() > 0)
		{
			mLock->unlock();
			AosXmlTagPtr xml = mMetadata->clone(AosMemoryCheckerArgsBegin);
			xml->setAttr("file_idx", mCrtScannerIdx);
			xml->setAttr("child_id", mScanners[mCrtScannerIdx]->getInstanceId());
			info->addMetadata(xml);
			return true;
		}

		// Did not read anything from the current file. Move on to the
		// next file.
		mCrtScannerIdx++;
	}

	// No more blocks to read anymore. 
	AosXmlTagPtr xml = mMetadata->clone(AosMemoryCheckerArgsBegin);
	if (!info) info = OmnNew AosBuffData();
	info->addMetadata(xml);
	mLock->unlock();
	return true;
}


bool 
AosDataScannerCompound::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	buff->setU32(mScanners.size());
	for (u32 i=0; i<mScanners.size(); i++)
	{
		mScanners[i]->serializeTo(buff, rdata);
	}
	return false;
}


bool 
AosDataScannerCompound::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	mScanners.clear();
	u32 size = buff->getU32(0);
	for (u32 i=0; i<size; i++)
	{
		AosDataScannerObjPtr scanner = AosCreateDataScanner(rdata, buff);
		aos_assert_rr(scanner, rdata, false);
		mScanners.push_back(scanner);
	}
	return false;
}
#endif
