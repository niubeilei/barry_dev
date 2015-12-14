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
// 03/27/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClientWrap_IILLogBuff_h
#define AOS_IILClientWrap_IILLogBuff_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/SPtr.h"
#include "Util/Buff.h"
#include "IILClient/Ptrs.h"

#include <algorithm>
#include <vector>
using namespace std;

class AosIILLogBuff : virtual public OmnRCObject
{
	OmnDefineRCObject;
	friend class AosIILHit;
	friend class AosIILStr;
	friend class AosIILU64;

private:
	enum
	{
		eInvalid = 0,
		
		eHit,
		eStr,
		eU64,

		eMax
	};

	int 				mIILType;
	u64 				mIILID;
	vector<AosBuffPtr> 	mVector;

	struct U64_SORT_OPR 
	{
		bool operator ()(const AosBuffPtr &s1, const AosBuffPtr &s2)
		{
			return s1->getU64(0) < s2->getU64(0);
		}
	} u64_sort_opr;
	
	struct STR_SORT_OPR 
	{
		bool operator ()(const AosBuffPtr &s1, const AosBuffPtr &s2)
		{
			return s1->getStr("") < s2->getStr("");
		}
	} str_sort_opr;
	
public:

	AosIILLogBuff();
	~AosIILLogBuff();

	bool	addEntry(const u32 crt_pos, const u32 size, const OmnFilePtr &file);
	bool	sortEntries();
	int		getNumEntries(){return mVector.size();}
	void	reset()
	{
		mIILType = eInvalid;
		mIILID = 0;
		mVector.clear();
	}
};

#endif

