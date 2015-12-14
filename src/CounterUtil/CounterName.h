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
// Modification History:
// 06/15/211: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_CounterName_h
#define Aos_CounterUtil_CounterName_h

#include "alarm_c/alarm.h"
#include "CounterUtil/CounterUtil.h"
#include "DocUtil/DocUtil.h"
#include "SEUtil/DocTags.h"
#include "SEBase/SeUtil.h"
#include "Util/StrSplit.h"
#include "Util/String.h"

#include <vector>
using namespace std;

class AosCounterName
{
private:
	enum
	{
		eMaxSegments = 1000
	};

	u32 				mIdx;
	vector<OmnString>	mMembers;
	vector<OmnString>	mContainers;
	OmnString			mCounterId;
	bool				mCounterIdUsed;
	OmnString 			mCname;
	vector<int>			mLevel;

public:
	AosCounterName();
	AosCounterName(const OmnString &name);
	~AosCounterName(){}

	// OmnString nextName()
	// {
	// 	if (mIdx >= mSegments.size()) return "";
	// }

	bool setName(const OmnString &name);
	bool setName2(const OmnString &name);

	bool nextName(OmnString &container, OmnString &member)
	{
		if (mIdx == mContainers.size()) 
		{
			//if (mCounterIdUsed) return false;
			//mCounterIdUsed = true;
			//container = mCounterId;
			//member = mMembers[mMembers.size()-1];
			if (mContainers.size() == 0)
			{
				container = mCname;
			}
			else
			{
				container = mContainers[mContainers.size()-1];
			}
			member = "";
			mIdx++;
			return true;
		}

		if (mIdx > mContainers.size() || mIdx > mMembers.size()) return false;

		container = mContainers[mIdx];
		member = mMembers[mIdx];
		mIdx++;
		return true;
	}

	bool nextName2(OmnString &container, OmnString &member, int &level)
	{
		if (mIdx == mContainers.size()) 
		{
			if (mContainers.size() == 0)
			{
				container = mCname;
				level = 1;
			}
			else
			{
				container = mContainers[mContainers.size()-1];
				level = mLevel[mLevel.size() -1];
			}
			member = "";
			mIdx++;
			return true;
		}

		if (mIdx > mContainers.size() || mIdx > mMembers.size()) return false;

		container = mContainers[mIdx];
		member = mMembers[mIdx];
		level = mLevel[mIdx];
		mIdx++;
		return true;
	}


	void reset() {mIdx = 0;}
	
	/*
	static bool isTypedName(const OmnString &cname)
	{
		//xxxx.xxxx__u64.docid
		//aa.bb.cc.const__u64.docid
		vector<OmnString> attrnames;
		aos_assert_r(parseStr(cname, AOSTAG_COUNTER_SEP, attrnames), false);
		aos_assert_r(attrnames.size() > 0, false);
		int idx = attrnames.size()-2;
		aos_assert_r(idx >= 0 && idx < (int) attrnames.size(), false);
		aos_assert_r(attrnames[idx] != "", false);
		OmnString name = attrnames[idx];
		
		int attrtype = (int) eAosAttrType_Str;
		if (AosDocUtil::isTypedAttr(name.data(), name.length()))
		{
		    attrtype = (int)name.data()[name.length()-1];
		}
		
	    if (attrtype == eAosAttrType_U64)return true;

		return false;
	}
	*/
};
#endif

