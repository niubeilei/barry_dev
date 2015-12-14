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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_QueryGroupFilter_h
#define Aos_QueryUtil_QueryGroupFilter_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "QueryUtil/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEUtil/SeTypes.h"
#include "SEBase/SeUtil.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/SeXmlParser.h"



class AosQueryGroupFilter : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum ValueType
	{
		eStr = 1,
		eU64 = 2
	};

private:
	AosQueryContextObj::GroupFilterType 	mGroupFilterType;
	AosOpr				mOpr;
	// Chen Ding, 2014/02/26
	// OmnString			mStrValue;
	// u64					mU64Value;
	OmnString			mStrValue1;
	OmnString			mStrValue2;
	u64					mU64Value1;
	u64					mU64Value2;
	OmnString			mSaperator;
	bool				mCreateDefault;
	ValueType			mValueType;	

public:
	AosQueryGroupFilter();
	AosQueryGroupFilter(const AosXmlTagPtr &xml);
	AosQueryGroupFilter(
			const AosQueryContextObj::GroupFilterType filterType,
			const AosOpr	 opr,
			const OmnString	 &value1,
			const OmnString	 &value2,
			const OmnString	 &saperator,
			const bool	     createDefault); 
	AosQueryGroupFilter(
			const AosQueryContextObj::GroupFilterType filterType,
			const AosOpr	 opr,
			const u64		 &value1,
			const u64		 &value2,
			const OmnString	 &saperator,
			const bool	     createDefault); 
	AosQueryGroupFilter(const AosQueryGroupFilterPtr &rhs);
	
	~AosQueryGroupFilter();
	
	OmnString	toString()const;
	bool		isValid(const OmnString &value, bool &createDefault, const bool num_alpha);
	bool		isValid(const u64 &value,bool &createDefault);
	void		setSaperator(const OmnString &saperator){mSaperator = saperator;}
	AosQueryGroupFilterPtr clone() const;
};
#endif



