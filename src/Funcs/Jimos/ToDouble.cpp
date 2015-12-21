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
// JIMODB-1347
// 2015/12/16 Created by Pay Zhang 
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/ToDouble.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosToDouble_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosToDouble(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosToDouble::AosToDouble(const int version)
:
AosGenFunc("ToDouble", version)
{
}

AosToDouble::AosToDouble()
:
AosGenFunc("ToDouble", 1)
{
}



AosToDouble::AosToDouble(const AosToDouble &rhs)
:
AosGenFunc(rhs)
{
}


AosToDouble::~AosToDouble()
{
}


bool
AosToDouble::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosToDouble::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosToDouble::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	AosValueRslt vv;
	AosDataType::E type;
	OmnString mType;
	bool rslt;
	double num;

	if(mSize > 1) return false;
	
	aos_assert_r(mSize == 1,false);

	rslt = mParms[0]->getValue(rdata, record, vv);
	aos_assert_r(rslt ,false);

	type = vv.getType();
	switch(type)
	{
		case AosDataType::eU64:
			{
				num = vv.getU64();
				break;
			}
		case AosDataType::eInt64:
			{
				num = vv.getI64();
				break;
			}
		case AosDataType::eDouble:
			{
				num = vv.getDouble();
				break;
			}
		defalut:
			{
				OmnAlarm << "Invalid Datatype : " << AosDataType::getTypeStr(type)<< "value : " << vv.getStr() << enderr;
				return false;
			}
	}
	value.setDouble(num);

	return true;
}

bool 
AosToDouble::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	
	if (mSize > 1)
	{
		errmsg << "ToChar needs at 1 parameter.";
		return false;
	}
	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosToDouble::cloneJimo()  const
{
	try
	{
		return OmnNew AosToDouble(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

