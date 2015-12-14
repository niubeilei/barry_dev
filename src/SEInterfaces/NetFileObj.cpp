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
// 2015/07/07	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/NetFileObj.h"

AosDirInfo::AosDirInfo()
: mPhysicalId(-1)
{
}

AosDirInfo::AosDirInfo(const OmnString &dirname)
: mPhysicalId(-1),
mDirName(dirname)
{
}


bool	
AosDirInfo::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	mDirName = buff->getOmnStr("");
	aos_assert_r(mDirName != "" , false);
	return true;
}


bool	
AosDirInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(mDirName != "", false);
	buff->setOmnStr(mDirName);
	return true;
}

	
bool	
AosDirInfo::serializeFrom(const AosXmlTagPtr &xml)
{
	aos_assert_r(xml, false);
	mDirName = xml->getNodeText();
	aos_assert_r(mDirName != "", false);
	return true;
}


OmnString	
AosDirInfo::serializeToXmlStr()
{
	aos_assert_r(mDirName != "", "");
	OmnString str;
	str << "<file "
		<< "<![CDATA[" << mDirName << "]]></file>"; 
	return str;
}


AosFileInfo::AosFileInfo()
:
mFileId(0), 
mFileName(""),
mFileLen(0),
mTotalLen(0),
mCrtBlockIdx(0), 
mStartOffset(0), 
mPhysicalId(-1),
mShuffleId(-1),
mFromTaskDocid(0),
mDataColId(""),
mTaskDataType(""),
mLevel(-1)
{
}

AosFileInfo::AosFileInfo(struct stat &st)
:
mFileId(0), 
mFileName(""),
mFileLen(st.st_size),
mTotalLen(mFileLen),
mCrtBlockIdx(0), 
mStartOffset(0), 
mPhysicalId(-1),
mShuffleId(-1),
mFromTaskDocid(0),
mDataColId(""),
mTaskDataType(""),
mLevel(-1)
{
}

	
bool	
AosFileInfo::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	mFileName = buff->getOmnStr("");
	mFileLen = buff->getI64(-1);
	mTotalLen = buff->getI64(-1);
	mCrtBlockIdx = buff->getInt(0);

	aos_assert_r(mFileName != "" && mFileLen > 0, false);
	mFromTaskDocid = buff->getU64(0);
	mDataColId = buff->getOmnStr("");
	mTaskDataType = buff->getOmnStr("");
	mShuffleId = buff->getI64(-1);
	mLevel = buff->getInt(-1);
	return true;
}


bool	
AosFileInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	aos_assert_r(mFileName != "" && mFileLen > 0, false);
	
	buff->setOmnStr(mFileName);
	buff->setI64(mFileLen);
	buff->setI64(mTotalLen);
	buff->setInt(mCrtBlockIdx);
	buff->setU64(mFromTaskDocid);
	buff->setOmnStr(mDataColId);
	buff->setOmnStr(mTaskDataType);
	buff->setI64(mShuffleId);
	buff->setInt(mLevel);
	return true;
}


bool	
AosFileInfo::serializeFrom(const AosXmlTagPtr &xml)
{
	aos_assert_r(xml, false);

	mFileId = xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	mFileName = xml->getNodeText();
	mFileLen = xml->getAttrInt64(AOSTAG_LENGTH, -1);
	mTotalLen = mFileLen;
	mCrtBlockIdx = xml->getAttrInt(AOSTAG_BLOCK_IDX, 0);
	mStartOffset = xml->getAttrInt64(AOSTAG_START_POS, 0);
	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mCharset = xml->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	mFromTaskDocid = xml->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
	mDataColId = xml->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	mTaskDataType = xml->getAttrStr(AOSTAG_TYPE);
	mShuffleId = xml->getAttrInt64(AOSTAG_SHUFFLE_ID, -1);
	mLevel = xml->getAttrInt(AOSTAG_LEVEL, -1);
	
	AosXmlTagPtr block = xml->getFirstChild(true);
	while(block)
	{
		AosFileInfo info;
		info.serializeFrom(block);
		mFileBlocks[info.mCrtBlockIdx] = info;
		block = xml->getNextChild();
	}
	return true;
}

OmnString	
AosFileInfo::serializeToXmlStr()
{
	OmnString str;
	str << "<file "
		<< AOSTAG_STORAGE_FILEID << "=\"" << mFileId << "\" "
		<< AOSTAG_LENGTH << "=\"" << mFileLen << "\" "
		<< AOSTAG_LEVEL << "=\"" << mLevel << "\" "
		<< AOSTAG_TOTAL_LENGTH << "=\"" << mTotalLen << "\" "
		<< AOSTAG_BLOCK_IDX << "=\"" << mCrtBlockIdx << "\" "
		<< AOSTAG_START_POS << "=\"" << mStartOffset << "\" " 
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< AOSTAG_CHARACTER << "=\"" << mCharset << "\" "
		<< AOSTAG_FROM_TASK_DOCID << "=\"" << mFromTaskDocid << "\" "
		<< AOSTAG_DATACOLLECTOR_ID << "=\"" << mDataColId << "\" "
		<< AOSTAG_SHUFFLE_ID << "=\"" << mShuffleId << "\" "
		<< AOSTAG_TYPE << "=\"" << mTaskDataType << "\" ";
	if (mFileName != "")
	{
		str << "><![CDATA[" << mFileName << "]]></file>"; 
	} 
	else if (!mFileBlocks.empty())
	{
		str << ">";
		for (u32 i=0; i<mFileBlocks.size(); i++)
		{
			str << mFileBlocks[i].serializeToXmlStr();
		}
		str << "</file>";
	}
	else
	{
		str << "/>";
	}
	return str;
}



bool	
AosNetFileObj::append(
			const char *buff,
			const int len,
			const bool flush)
{
	return false;
}

int64_t	
AosNetFileObj::read(
			vector<AosBuffPtr> &buffs,
			const int64_t offset,
			const int64_t len,
			const AosRundataPtr &rdata)
{
	return false;
}

int64_t	
AosNetFileObj::length(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return -1;
}

bool	
AosNetFileObj::readData(
			const u64 &reqId, 
			const int64_t &blocksize,
			AosRundata *rdata)
{ 
	OmnNotImplementedYet;
	return false;
}

void	
AosNetFileObj::setCaller(const AosFileReadListenerPtr &caller)
{
	OmnNotImplementedYet;
}

AosFileReadListenerPtr	
AosNetFileObj::getCaller()
{
	OmnNotImplementedYet;
	return 0;
}

void	
AosNetFileObj::seek(const int64_t start_pos)
{
	OmnNotImplementedYet;
}

void	
AosNetFileObj::moveToFront(const int64_t pos)
{
	OmnNotImplementedYet;
}

AosBuffPtr	
AosNetFileObj::getBuff()
{
	OmnNotImplementedYet;
	return 0;
}

int 
AosNetFileObj::getServerId()
{
	OmnNotImplementedYet;
	return -1;
}

u64 
AosNetFileObj::getFileId()
{
	OmnNotImplementedYet;
	return 0;
}

