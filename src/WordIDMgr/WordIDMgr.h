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
// 	Created: 10/15/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_WordIDMgr_WordIDMgr_h
#define Aos_WordIDMgr_WordIDMgr_h

#include "Alarm/Alarm.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArrayVar.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/Opr.h"


using namespace std;


class AosWordIDMgr: public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		eMaxBuffLen = 424,
		eMaxStrLen = 400,
		eWordArrayWordOffset = 24,
		eIDArrayWordOffset = 8,
		eMaxSortID = 0x7fffffffffffffff
	};
private:
/*       const string *m_word;
int m_word_id;
int m_sort_id;
*/

	AosBuffArrayVarPtr			mWordArray;
	AosBuffArrayVarPtr			mIDArray;

	u64							mCurWordID;
	AosBuffPtr					mTmpValue_a;
	AosBuffPtr					mTmpValue_b;
	char*						mTmpPtrA;
	u64*						mTmpPtrA2;
	char*						mTmpPtrB;
	u64*						mTmpPtrB2;
	
	u64							mClientTimeStamp;
	OmnMutexPtr					mLock;
	AosRundataPtr				mRundata;	
		
	
public:
	AosWordIDMgr();
	~AosWordIDMgr();
	u32 getWordIDPriv(const OmnString &word);
	void lock();
	void unlock();

	OmnString getWord(const u32 &word_id);
	u32 getWordIDSafe(const OmnString &word);

	bool saveWordMgr(const AosBuffPtr &buff);
	bool loadWordMgr(const AosBuffPtr &buff);
	void clear();
	u64  getTimeStamp();
	bool exportData(const u64 &timestamp, u64 &cur_timpstamp, const AosBuffPtr &buff);
	bool importData(const AosBuffPtr &buff, const u64 &cur_timestamp);
	int compareWord(const u32 &id1, const u32 &id2);
private:
	u32 	createWord(const OmnString &word, const i64 &index);
	//void	modify(node, mod_list, const bool flag);
	u64 	generateWordID();
	
};

#endif

