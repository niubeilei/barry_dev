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
// 02/22/2013 Created by Linda  
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_FieldFilter_h
#define Aos_QueryUtil_FieldFilter_h

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


class AosFieldFilter : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Type
	{
		eInvalid,
		eStatValue,
		eMax
	};

private:
	struct AosStatValue
	{
		//statvalue:u16 + term0 + 0x01 + term1 + 0x01 + term2(u16country0x01province0x01city) 
		int 	stat_field_idx;
		AosOpr	opr;
	};

private:
	Type				mType;
	AosStatValue		mStatValue;	
	vector<OmnString>	mFieldStr;

public:
	AosFieldFilter();
	AosFieldFilter(const OmnString &type){mType = toEnum(type);}
	AosFieldFilter(const AosXmlTagPtr &xml);
	~AosFieldFilter();

	OmnString 	getTypeStr(const Type type);

	Type 		toEnum(const OmnString &type);

	OmnString 	toString();

	bool		cond(const OmnString &key);

	void 		setStatValue(const int &stat_field_idx, const AosOpr &opr);

	void		addFieldStr(const OmnString &str);
	

};
#endif



