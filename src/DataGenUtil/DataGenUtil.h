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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataGenUtil_DataGenUtil_h
#define AOS_DataGenUtil_DataGenUtil_h

#include "DataGenUtil/DataGenUtilType.h"
#include "DataGenUtil/Ptrs.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"



class AosDataGenUtil : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosDataGenUtilType::E	mType;
	OmnMutexPtr				mLock;

public:
	enum
	{
		eMax = 100
	};

	AosDataGenUtil(
			const OmnString &name, 
			const AosDataGenUtilType::E type, 
			const bool reg);
	~AosDataGenUtil();

	virtual bool nextValue(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata) = 0;

	virtual bool nextValue(
			AosValueRslt &value,
			const AosRundataPtr &rdata) = 0;

	static bool nextValueStatic(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	AosDataGenUtilType::E getType() const
	{
		return mType;
	}

	virtual AosDataGenUtilPtr clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata) = 0;

	static AosDataGenUtilPtr getDataGenUtilStatic(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool registerDataGen(const OmnString &name, const AosDataGenUtilPtr &uu);
};
#endif

