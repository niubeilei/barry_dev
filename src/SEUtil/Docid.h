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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_Docid_h
#define AOS_SEUtil_Docid_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/Cloudid.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeErrors.h"
#include "Util/String.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !! 			System Reserved Docids						!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define AOSDOCID_APPROOT  				101
#define AOSDOCID_DELOBJROOT  			102
#define AOSDOCID_IILMGRROOT  			103

#define AOSDOCID_ROOTCTNR	  			300  //create default doc when the sever initialize.
#define AOSDOCID_SYSROOT 				301
#define AOSDOCID_SYSUSERCTNR 			302
#define AOSDOCID_CIDCTNR 				303
#define AOSDOCID_USRDOMAINCTNR 			304
#define AOSDOCID_MONITORLOGCTNR			305  
#define AOSDOCID_TEMPDOCCTNR			306  
#define AOSDOCID_ROOT	  				307  
#define AOSDOCID_GUEST	  				308  
#define AOSDOCID_UNKNOW	  				309  
#define AOSDOCID_IDGEN_CONTAINER		310
#define AOSDOCID_LOSTFOUND_CONTAINER 	311
#define AOSDOCID_SYSLOG_CONTAINER	 	312
#define AOSDOCID_SYSURL					313
#define AOSDOCID_DICT_PARENT			314
#define AOSDOCID_VIRTUALFILE_CONTAINER  315
#define AOSDOCID_SYSTEM_DEFAULT_ACD		316

// The docids [400-700] are reserved for Dictionary Containers
#define AOSDOCID_DICT_START				400	
#define AOSDOCID_DICT_END				700	

#define AOSDOCID_MAX_SYS_DOCID			1000

// !!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!
// The docids [10000, 10000+65535] are reserved for IILID GEN
// DO NOT USE THEM FOR OTHER PURPOSES.
#define AOSSTORAGEDOCID_IILID_IDGEN     10000		// Copied from StorageDocids.h
#define AOSSTORAGEDOCID_IILID_IDGEN_END 10000+65535	// Copied from StorageDocids.h
							


const int sgMaxDocidStrLen = 20;

class AosDocid
{
	enum
	{
		eDftInitDocid = 1000000
	};

	static u64 	mDftInitDocid;

public:
	static u64 convertToU64(const OmnString &docidstr)
	{
		const int len = docidstr.length();
		if (len <= 0 || len > sgMaxDocidStrLen) return AOS_INVDID;

		const char *data = docidstr.data();
		for (int i=0; i<len; i++) 
		{
			if (data[i] < '0' || data[i] > '9') return AOS_INVDID;
		}

		return atoll(data);
	}

	inline static u64 getDftInitDocid()
	{
		return mDftInitDocid;
	}

	inline static u64 getBitmapCacheIdPrefix()
	{
		return 0x3f3f3f3f00000000ULL;
	}	

	inline static u64 getBitmapModifyIdPrefix()
	{
		return 0x3e3e3e3e00000000ULL;
	}	
};

#endif
