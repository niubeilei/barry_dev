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
#include "Funcs/Jimos/Greatest.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGreatest_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGreatest(version);
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


AosGreatest::AosGreatest(const int version)
:
AosGenFunc("Greatest", version)
{
}

AosGreatest::AosGreatest()
:
AosGenFunc("Greatest", 1)
{
}



AosGreatest::AosGreatest(const AosGreatest &rhs)
:
AosGenFunc(rhs)
{
}


AosGreatest::~AosGreatest()
{
}


bool
AosGreatest::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosGreatest::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	OmnNotImplementedYet;
	return AosDataType::eDouble;
}


bool
AosGreatest::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	bool found = false;
	value.setNull();
	for (u32 i=0; i<mSize; i++)
	{
		AosGenFunc::getValue(rdata, i, record);
		if (mValue.isNull())
		{
			continue;
		}

		if (!found)
		{
			value = mValue;
			found = true;
		}
		else 
		{
			// by andy 
			if (!mValue.isNull() && !value.isNull())
			{
				AosDataType::E type = AosDataType::autoTypeConvert(mValue.getType(), value.getType());
				if (AosValueRslt::doComparison(eAosOpr_gt, type, mValue, value)) value = mValue;
			}
		}
	}
	return true;
}


bool 
AosGreatest::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "Greatest needs at least 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosGreatest::cloneJimo()  const
{
	try
	{
		return OmnNew AosGreatest(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

