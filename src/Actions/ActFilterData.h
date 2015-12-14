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
// 05/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActFilterData_h
#define Aos_SdocAction_ActFilterData_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"


class AosActFilterData : virtual public AosSdocAction
{
private:
	enum MapType
	{
		eInvalidMap,
		eBlackMap,
		eWhiteMap
	};

	AosCharS2U8_t	mMap;
	MapType			mMapType;
	int				mStartPos;
	int				mMatchedLength;
	OmnMutexPtr		mLock;

public:
	AosActFilterData(const bool flag);
	AosActFilterData(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActFilterData();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool run(const char *&data, int &len, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

};
#endif

