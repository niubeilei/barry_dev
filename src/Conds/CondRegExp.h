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
#ifndef Aos_SdocAction_CondRegExp_h
#define Aos_SdocAction_CondRegExp_h

#include "Conds/Condition.h"
#include "Util/ValueRslt.h"


class AosCondRegExp : virtual public AosCondition
{
private:
	enum MapType
	{                           
		eInvalidMap,
		eBlackMap,
		eWhiteMap 
	};

	MapType         mMapType;
	OmnString	 	mRegExp;

public:
	AosCondRegExp(const bool flag);
	AosCondRegExp(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosCondRegExp();

	virtual bool evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool evalCond(const char *data, const int len, const AosRundataPtr &rdata);
	virtual AosConditionObjPtr clone();

	virtual bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

