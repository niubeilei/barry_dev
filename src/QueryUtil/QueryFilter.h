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
// 2014/08/19,  Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_QueryFilter_h
#define Aos_QueryUtil_QueryFilter_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "QueryUtil/Ptrs.h"
#include "SEInterfaces/ConditionObj.h"
#include "SEInterfaces/QueryFilterObj.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "Util/Opr.h"


class AosQueryFilter : public AosQueryFilterObj
{
public:
	FilterType	mFilterType;

public:
	AosQueryFilter(const FilterType type):mFilterType(type){}
	~AosQueryFilter(){}

	static AosQueryFilterObjPtr	create(
									const AosXmlTagPtr &def,
									const AosRundataPtr &rdata);

	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const {return false;}
	virtual bool	evalFilter(
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata) const {return false;} 
};


class AosQueryFilterValue : public AosQueryFilter
{
public:
	AosOpr		mOpr;
	u64			mValue1;
	u64			mValue2;

	AosQueryFilterValue():AosQueryFilter(eValue){}
	~AosQueryFilterValue(){}

	static AosQueryFilterObjPtr	create(
									const AosXmlTagPtr &def,
									const AosRundataPtr &rdata);

	virtual AosQueryFilterObjPtr clone() const;
	virtual OmnString	toXmlString() const;

	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const; 
	virtual bool	evalFilter(
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata) const; 

};


class AosQueryFilterStrKey : public AosQueryFilter
{
public:
	AosOpr				mOpr;
	OmnString			mValue1;
	OmnString			mValue2;
	AosConditionObjPtr	mCond;
	AosXmlTagPtr		mCondDef;

	AosQueryFilterStrKey():AosQueryFilter(eStrKey){}
	~AosQueryFilterStrKey(){}

	static AosQueryFilterObjPtr	create(
									const AosXmlTagPtr &def,
									const AosRundataPtr &rdata);

	virtual AosQueryFilterObjPtr clone() const;
	virtual OmnString	toXmlString() const;

	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const; 
};


class AosQueryFilterU64Key : public AosQueryFilter
{
public:
	AosOpr		mOpr;
	u64			mValue1;
	u64			mValue2;

	AosQueryFilterU64Key():AosQueryFilter(eU64Key){}
	~AosQueryFilterU64Key(){}

	static AosQueryFilterObjPtr	create(
									const AosXmlTagPtr &def,
									const AosRundataPtr &rdata);

	virtual AosQueryFilterObjPtr clone() const;
	virtual OmnString	toXmlString() const;

	virtual bool	evalFilter(
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata) const; 
};


class AosQueryFilterKeyField : public AosQueryFilter
{
public:
	OmnString			mSep;
	int					mFieldIdx;

	AosOpr				mOpr;
	OmnString			mValue1;
	OmnString			mValue2;
	AosConditionObjPtr	mCond;
	AosXmlTagPtr		mCondDef;

	AosQueryFilterKeyField():AosQueryFilter(eKeyField){}
	~AosQueryFilterKeyField(){}

	static AosQueryFilterObjPtr	create(
									const AosXmlTagPtr &def,
									const AosRundataPtr &rdata);

	virtual AosQueryFilterObjPtr clone() const;
	virtual OmnString	toXmlString() const;

	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const; 
};

#endif



