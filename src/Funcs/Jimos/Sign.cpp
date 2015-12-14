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
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/Sign.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSign_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSign(version);
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


AosSign::AosSign(const int version)
:
AosGenFunc("Sign", version)
{
}

AosSign::AosSign()
:
AosGenFunc("Sign", 1)
{
}



AosSign::AosSign(const AosSign &rhs)
:
AosGenFunc(rhs)
{
}


AosSign::~AosSign()
{
}


bool
AosSign::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosSign::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	if (mValue.getType() == AosDataType::eInt64)
	{
		int data_int  = mValue.getI64();
		if (data_int > 0)
		{
			i64 result = 1;
			value.setI64(result);
		}
		else if(data_int == 0)
		{
			i64 result = 0;
			value.setI64(result);
		}
		else
		{
			i64 result = -1;
			value.setI64(result);
		}
		return true;
	}
	else if (mValue.getType() == AosDataType::eDouble)
	{
		double data_double = mValue.getDouble();
		i64 result = 0;
		if(data_double > 0)
		{
			result = 1;
			value.setI64(result);
		}
		else if(data_double == 0)
		{
			result = 0;
			value.setI64(result);
		}
		else
		{
			result = -1;
			value.setI64(result);
		}
		return true;
	}
	else if (mValue.getType() == AosDataType::eU64)
	{
		u64 data_u = mValue.getU64();
		i64 result = 0;
		if(data_u > 0)
		{
			result = 1;
			value.setI64(result);
		}
		else if(data_u == 0)
		{
			result = 0;
			value.setI64(result);
		}
		else
		{
			result = -1;
			value.setI64(result);
		}
		return true;
	}

	return false;
}

bool 
AosSign::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Sign needs at 1 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosSign::cloneJimo()  const
{
	try
	{
		return OmnNew AosSign(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

