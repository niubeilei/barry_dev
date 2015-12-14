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
#include "Funcs/Jimos/Decode.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDecode_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDecode(version);
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


AosDecode::AosDecode(const int version)
:
AosGenFunc("Decode", version)
{
}

AosDecode::AosDecode()
:
AosGenFunc("Decode", 1)
{
}



AosDecode::AosDecode(const AosDecode &rhs)
:
AosGenFunc(rhs)
{
}


AosDecode::~AosDecode()
{
}


bool
AosDecode::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDecode::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	OmnNotImplementedYet;
	return AosDataType::eInvalid;
}


bool
AosDecode::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	AosValueRslt vv1;
	if (mSize % 2 == 1)
	{
		value.setNull();
	}
	else
	{
		if (!AosGenFunc::getValue(rdata, mSize -1, record)) return false;
		value = mValue;
	}

	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	vv1 = mValue; 

	for (size_t i=1; i<mSize-1; i+=2)
	{
		// by andy
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		if (mValue.isNull() || vv1.isNull()) return false;
		AosDataType::E type = AosDataType::autoTypeConvert(mValue.getType(), vv1.getType());
		if (AosValueRslt::doComparison(eAosOpr_eq, type, mValue, vv1))
		{
			if (!AosGenFunc::getValue(rdata, i+1, record)) return false;
			value = mValue;
			return true;
		}
	}

	return true;
}


bool 
AosDecode::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	//if (parms.size() < 3 || parms.size() % 2 != 0)
	if(mSize < 3)
	{
		errmsg << "ERROR: please check decode parameter's number";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDecode::cloneJimo()  const
{
	try
	{
		return OmnNew AosDecode(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

