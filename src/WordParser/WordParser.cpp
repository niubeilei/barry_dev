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
// 09/18/2010	Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "WordParser/WordParser.h"

#include <scws/scws.h>
#include <string.h>
#include "Util/File.h"


// static scws_t mScws;
static OmnString sgScwsDictFname = "/usr/local/scws/etc/dict.utf8.xdb";
static OmnString sgScwsIniFname = "/usr/local/scws/etc/rules.utf8.ini";
static OmnString sgScwsCustFname = "/usr/local/scws/etc/keyword.txt";

/*
bool
AosWordParser::init(const AosXmlTagPtr &config)
{
	if (config)
	{
		AosXmlTagPtr scwsconfig = config->getFirstChild(AOSCONFIG_SCWS);
		if (scwsconfig)
		{
			sgScwsDictFname = scwsconfig->getAttrStr(AOSCONFIG_DICTFNAME, sgScwsDictFname);
			sgScwsIniFname = scwsconfig->getAttrStr(AOSCONFIG_DICTINIFNAME, sgScwsIniFname);
			sgScwsCustFname = scwsconfig->getAttrStr(AOSCONFIG_CUSTFNAME, sgScwsCustFname);
		}
	}
	return true;
}
*/


AosWordParser::AosWordParser()
:
mRes(0),
mCrt(0),
mFirst(false),
mFinished(false),
mScws(0)
{
	if (!(mScws = scws_new())) 
	{
	    OmnAlarm << "error, can't init the scws_t!" << enderr;
		return;
	}
	scws_set_charset(mScws, "utf-8");
	scws_set_dict(mScws, sgScwsDictFname.data(), SCWS_XDICT_XDB);
	static FILE *ff = OmnFile::openFile(sgScwsDictFname, OmnFile::eReadOnly); 
	if (!ff)
	{
		OmnAlarm << "Miss Dictionarys!" << enderr; 
		exit(1);
	}
	scws_set_rule(mScws, sgScwsIniFname.data());
	scws_add_dict(mScws, sgScwsCustFname.data(), SCWS_XDICT_TXT);
	scws_set_multi(mScws, SCWS_MULTI_SHORT);
	scws_set_ignore(mScws, 1);
}


AosWordParser::~AosWordParser()
{
	scws_free(mScws);
}


bool
AosWordParser::setSrc(const OmnString &src)
{
	mCrtStr = src;
    scws_send_text(mScws, mCrtStr.data(), mCrtStr.length());
	mFirst = true;
	mFinished = false;
	return true;
}


bool
AosWordParser::setSrc(const char* data, int start, int len)
{
	aos_assert_r(data, false);
	aos_assert_r(start >= 0, false);
	aos_assert_r(len > 0, false);
	OmnString src(&data[start], len);
	mCrtStr = src;
    scws_send_text(mScws, mCrtStr.data(), mCrtStr.length());
	mFirst = true;
	mFinished = false;
	return true;
}


bool
AosWordParser::nextWord(OmnString &wordbuff)
{
	if (mFirst)
	{
    	mRes = mCrt = scws_get_result(mScws);
		mFirst = false;
	}
	if (!mCrt) 
	{
		scws_free_result(mRes);
		mRes = mCrt = scws_get_result(mScws);
		if (!mCrt)
		{
			mFinished = true;
			return false;
		}
	}
	int end = mCrt->off + mCrt->len - 1;
	//check word 
	if (end == 0) 
		wordbuff << (mCrtStr.getBuffer())[0];
	else
		wordbuff = mCrtStr.substr(mCrt->off, end);
	mCrt = mCrt->next;
	return true;
}


bool
AosWordParser::nextWord(char *wordbuff, int &len)
{
	if (mFirst)
	{
    	mRes = mCrt = scws_get_result(mScws);
		mFirst = false;
	}
	if (!mCrt) 
	{
		scws_free_result(mRes);
		mRes = mCrt = scws_get_result(mScws);
		if (!mCrt)
		{
			mFinished = true;
			return false;
		}
	}
	int end = mCrt->off + mCrt->len - 1;
	if (end == 0)
	{
		memcpy(wordbuff, mCrtStr.getBuffer(), 1);
		len = 1;
	}
	else
	{
		char * ss = mCrtStr.getBuffer();
		memcpy(wordbuff, &ss[mCrt->off], mCrt->len);
		len = mCrt->len;
	}
	mCrt = mCrt->next;
	return true;
}
