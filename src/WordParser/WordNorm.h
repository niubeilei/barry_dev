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

#ifndef Aos_WordMgr_WordNorm_h
#define Aos_WordMgr_WordNorm_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SearchEngine/Ptrs.h"
#include "Thread/Mutex.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/OmnNew.h"


/*
struct eqstr
{
  	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) == 0;
	}
};
*/

class AosWordNorm : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eWordTablesize = 0xffff, 
		eNormTablesize = 0xffff
	};

	OmnMutexPtr			mLock;
	OmnString			mDirname;
	OmnString			mFilename;

public:
	AosWordNorm(const OmnString &, const OmnString &nfname);
	~AosWordNorm();

	char * normalizeWord(
					char *word, 
					int &wordLen, 
					char *buff, 
					const int bufflen);
	char * filterReservedPrefix(
					const char *word, 
					int &len, 
					char *buff, 
					const int bufflen)
	{
		if (len < 3 || word[0] != '_' || word[1] != 'z' || word[2] != 't') 
			return (char *)word;
		
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

	bool		 createTable();
};
#endif
