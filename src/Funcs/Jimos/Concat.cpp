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
#include "Funcs/Jimos/Concat.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosConcat_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosConcat(version);
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


AosConcat::AosConcat(const int version)
:
AosGenFunc("concat", version)
{
}

AosConcat::AosConcat()
:
AosGenFunc("concat", 1)
{
}



AosConcat::AosConcat(const AosConcat &rhs)
:
AosGenFunc(rhs)
{
}


AosConcat::~AosConcat()
{
}


bool
AosConcat::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}

AosDataType::E 
AosConcat::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosConcat::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString ss;
	for (u32 i=0; i<mSize; i++)
	{
		if (!(AosGenFunc::getValue(rdata, i, record))) return false;
		ss << mValue.getStr();
	}
	value.setStr(ss);
	return true;
}

bool 
AosConcat::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "Concat needs at least 2 parameters.";
		return false;
	}

	return true;
}

AosJimoPtr
AosConcat::cloneJimo()  const
{
	try
	{
		return OmnNew AosConcat(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

