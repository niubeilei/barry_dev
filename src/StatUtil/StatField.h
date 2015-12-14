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
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_StatField_h
#define AOS_StatUtil_StatField_h


#include "StatUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/DataTypes.h"
#include "XmlUtil/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Util/Number.h"
#include "SEInterfaces/ExprObj.h"
#include "API/AosApi.h"
#include "StatUtil/StatTimeUnit.h"

class AosStatField : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Type
	{
		eKeyField,
		eTimeField,
		eMeasureField,
		
		eInvalid,
	};

	OmnString 		mName;
	Type			mFieldType;	
	AosDataType::E	mDataType;	
	AosNumber		mNumber;
	//arvin 2015.07.29
	//JIMODB-114
	AosDataType::E 	mType;
	AosExprObjPtr	mCond;
	OmnString		mCondMeasureName;
	AosStatField(const Type tp)
	:
	mFieldType(tp)
	{}
	
	virtual Type	getFieldType(){ return mFieldType; };
	virtual AosDataType::E getDataType(){ return mDataType; };
	virtual void	setDataType(AosDataType::E tp){ mDataType = tp; };
	virtual OmnString getFieldName(){ return mName; };
	virtual void	setFieldName(const OmnString &fname){ mName = fname; };
	virtual AosStatTimeUnit::E getTimeUnit(){return AosStatTimeUnit::eInvalid;}
	//arvin  2015.07.29
	virtual void	setType(AosDataType::E type){ mType = type; };
	virtual void    setCondExpr(const AosExprObjPtr &expr){mCond = expr;};

	static AosStatFieldPtr configStatic(const AosXmlTagPtr &conf);
	static Type typeStrToEnum(const OmnString str);
	static OmnString typeEnumToStr(const Type tp);

	virtual bool config(const AosXmlTagPtr &conf) = 0;
	virtual OmnString toXmlString() = 0;
	virtual bool toXmlString(OmnString &str);

};

#endif
