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
#ifndef Aos_SdocAction_ActSumUniq_h
#define Aos_SdocAction_ActSumUniq_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/DataTypes.h"
#include "Util/CompareFun.h"
#include <map>
#include <algorithm>
#include <functional>

class AosActSumUniq : virtual public AosSdocAction
{

	enum E
	{
		eInvalid = 0,
		eU8 = 256,
		eU16 = 65535,
		eU64 = 0xffffffffffffffffULL
	};

	typedef map<char *, int, AosFunCustom> SumUniqMap;
	typedef map<char *, int, AosFunCustom>::iterator SumUniqMapItr;

private:
	OmnMutexPtr							mLock;
	SumUniqMap*							mMap;
	AosDataType::E						mKeyDataType;
	int 								mStartPos;
	int									mMatchedLength;
	AosFunCustom*						mComp;
	E									mCodingType;
	u64									mMappedValue;

public:
	AosActSumUniq(const bool flag);
	AosActSumUniq(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActSumUniq();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool run(const char *data, int &len, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
	bool finish(OmnString &objid, const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosActSumUniq::E toEnum(const OmnString &str)
	{
		if (str == "u8")  	return eU8;
		if (str == "u16") 	return eU16;
		if (str == "u64") 	return eU64;
		return eInvalid;
	}
	
	OmnString toString(const AosActSumUniq::E str)
	{
		if (str == eU8)  	return "u8";
		if (str == eU16) 	return "u16";
		if (str == eU64) 	return "u64";
		return "";
	}
};
#endif

