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
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_CondIILMap_h
#define Aos_SdocAction_CondIILMap_h

#include "Conds/Condition.h"
#include "Util/ValueRslt.h"

#include <map>

class AosCondIILStrMap : virtual public OmnRCObject, 
					  public map<OmnString, OmnString>
{
	OmnDefineRCObject;
	
	OmnMutexPtr		mLock;
	
	friend class AosCondIILMap;

public:
	AosCondIILStrMap()
	:
	mLock(OmnNew OmnMutex())
	{
	}
	~AosCondIILStrMap() {}
};


class AosCondIILMap : virtual public AosCondition
{
	enum MapType
	{
		eInvalidMap,
		eBlackMap,
		eWhiteMap
	};

	MapType				mMapType;
	AosCondIILStrMapPtr	mIILMap;
	AosXmlTagPtr		mIILScannerCfg;

public:
	AosCondIILMap(const bool flag);
	AosCondIILMap(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);
	~AosCondIILMap();

	virtual bool evalCond(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	virtual bool evalCond(
					const AosValueRslt &value,
					const AosRundataPtr &rdata);
	virtual bool evalCond(
					const char *data,
					const int len,
					const AosRundataPtr &rdata);
	virtual AosConditionObjPtr clone();

	virtual bool config(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);

private:
	bool		evalCond(
					const OmnString &value,
					const AosRundataPtr &rdata);
	bool		createIILMap(const AosRundataPtr &rdata);
};
#endif

