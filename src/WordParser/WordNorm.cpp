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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//
// Modification History:
// 10/17/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordParser/WordNorm.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEBase/SeUtil.h"
#include "Thread/Mutex.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "WordMgr/WordIdHash.h"



struct eqstr
{
	  bool operator()(const char* s1, const char* s2) const
		    {
				    return strcmp(s1, s2) == 0;
					  }
};


// static hash_map<const char *, const char *,
  //  	hash<const char*>, eqstr> sgWordHash;

AosWordNorm::AosWordNorm(
	const OmnString &dirname, 
	const OmnString &fname)
:
mLock(OmnNew OmnMutex()),
mDirname(dirname),
mFilename(fname)
{
	/*
	mWordHash = OmnNew AosWordIdHash(wfname, eWordTablesize, 0);
	mNormHash = OmnNew AosWordIdHash(nfname, eNormTablesize, 0);
	=======
	mWordHash = OmnNew AosWordIdHash(dirname, eWordTablesize, 0, wfname);
	mNormHash = OmnNew AosWordIdHash(dirname, eNormTablesize, 0, nfname);
	*/
}


AosWordNorm::~AosWordNorm()
{
}


char *
AosWordNorm::normalizeWord(
		char *word, 
		int &len, 
		char *buff, 
		const int bufflen)
{
	// This function checks whether the word has a normalized
	// form. If yes, return the normalized word and its
	// length. Otherwise, if the word starts with the reserved
	// prefix '_zt', it is prepended with '_zt00'. The converted
	// word is returned through 'buff', and the converted word
	// length is returned through 'len'. 
	// Otherwise, it returns the original word 'word'. 

	//aos_assert_r(len > 1, 0);

	// 1. Convert to lower cases
	// char c;
	// for (u32 i=0; i<len; i++)
	// {
	// 	c = word[i];
	// 	if (c >= 'A' && c <= 'Z') word[i] = c - 'A' + 'a';
	// }

	// 2. Check whether it can be normalized by the hashtable.
	// const char *str = sgWordHash[word];
	// if (str) 
	// {
	// 	len = strlen(str);
	// 	return str;
	// }
	
	// 3. Check whether it starts with the reserved prefix: '_zt'. 
	// If yes, it should prepend it with '_zt00'.
	if (len >= 3 && word[0] == '_' && word[1] == 'z' && word[2] == 't')
	{
		buff[0] = '_';
		buff[1] = 'z';
		buff[2] = 't';
		buff[3] = '0';
		buff[4] = '0';
		int maxlen = (len + 5 < bufflen)?len:bufflen-5;
		memcpy(&buff[5], word, maxlen);
		len = maxlen + 5;
		return buff;
	}

	return word;
}


bool
AosWordNorm::createTable()
{
	/*
	// The hashtable is defined in a file, in the form:
	// 		<orig_word>  <converted_word>
	// The file is 
	OmnString fname = mDirname;
	fname << "/" << mFilename;
	OmnFile ff(fname, OmnFile::eReadOnly);
	aos_assert_r(ff.isGood(), false);

	bool finished = false;
	while (!finished)
	{
		OmnString orig = ff.getLine(finished);
		aos_assert_r(orig.length() > 0, false);
		aos_assert_r(!finished, false);
		OmnString converted = ff.getLine(finished);
		aos_assert_r(converted.length() > 0, false);
		sgWordHash[orig.data()] = converted.data();
	}
	*/
	return true;
}

