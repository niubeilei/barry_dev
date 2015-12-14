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
//
// Modification History:
// 11/03/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SmartReply/SmartReplyMgr.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "StorageMgr/StorageMgr.h"
#include "XmlUtil/XmlTag.h"
#include <boost/regex.hpp>
using boost::regex;

OmnSingletonImpl(AosSmartReplyMgrSingleton,
				AosSmartReplyMgr,
				AosSmartReplyMgrSelf,
				"AosSmartReplyMgr");

AosSmartReplyMgr::AosSmartReplyMgr()
:
mCrtPatternNum(0)
{
	OmnNotImplementedYet;
	/*
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mPatternFile = AosStorageMgr::getSelf()->openFileByStrKey(AOSSTRKEY_SMARTREPLY_PATTERN, rdata);
	
	if(!mPatternFile)
	{
		mPatternFile = AosStorageMgr::getSelf()->createFileByStrKey(
				AOSSTRKEY_SMARTREPLY_PATTERN, eFileSize, true, rdata);
		aos_assert(mPatternFile);
	}
	else
	{
		// read the pattern from the file.
		u32 offset = 0;
		char data[eEntrySize];
		while(1)
		{
			int len = mPatternFile->readToBuff(offset, eEntrySize, data);
			if(len <= 0)	break;

			u32 pattern_len = *(u32 *)&data[ePatternLenOffset];
			OmnString pattern(&data[ePatternOffset], pattern_len);
			mPatterns.push_back(pattern);
			
			u32 answer_len = *(u32 *)&data[eAnswerLenOffset];
			OmnString answer(&data[eAnswerOffset], answer_len);
			mAnswers.push_back(answer);
			
			offset += eEntrySize;
		}

		//recover the mCrtPatternNum
		u64 crtFileSize = mPatternFile->getFileCrtSize();
		if(crtFileSize)
		{
			mCrtPatternNum = crtFileSize/eEntrySize + 1;
		}
	}
	*/
}


AosSmartReplyMgr::~AosSmartReplyMgr()
{
}

void
AosSmartReplyMgr::addPatterns(const OmnString &pattern, const OmnString &answer)
{
	aos_assert(pattern != "" && answer != "");
	
	OmnString ss = ".*";

	OmnStrParser1 parser(pattern, ",");
	OmnString key_word;
	while((key_word = parser.nextWord()) != "")
	{
		ss << key_word << ".*";	
	}
	mPatterns.push_back(ss);
	mAnswers.push_back(answer);

	//save to file.
	u32 offset =( mCrtPatternNum++) * eEntrySize;
	mPatternFile->setU32(offset+ePatternLenOffset, ss.length(), false);
	mPatternFile->setStr(offset+ePatternOffset, ss, false);
	mPatternFile->setU32(offset+eAnswerLenOffset, answer.length(), false);
	mPatternFile->setStr(offset+eAnswerOffset, answer, true);
}


OmnString
AosSmartReplyMgr::getReply(const OmnString &question)
{
	string s_ques(question.toString().data(), question.length());
	for(u32 i=0; i<mPatterns.size(); i++)
	{
		string s(mPatterns[i].toString().data(), mPatterns[i].length());
		regex reg(s);
		if(regex_match(s_ques, reg))
		{
			return mAnswers[i];
		}
	}

	return "No Answer!";
}
