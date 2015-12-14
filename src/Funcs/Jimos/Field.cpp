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
#include "Funcs/Jimos/Field.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosField_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosField(version);
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


AosField::AosField(const int version)
:
AosGenFunc("Field", version)
{
}

AosField::AosField()
:
AosGenFunc("Field", 1)
{
}



AosField::AosField(const AosField &rhs)
:
AosGenFunc(rhs)
{
}


AosField::~AosField()
{
}


bool
AosField::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosField::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;	
}


bool
AosField::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	u32 i = 0;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	string str = mValue.getStr();
	for(i=1;i<mSize;i++)
	{
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		string s= mValue.getStr();
		if(s==str)
		{
			int idx=i;
			value.setI64(idx);
			break;
		}
	}
	if(i==mSize)
	{
		value.setI64(0);
	}
	return true;
}

bool 
AosField::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize < 2)
	{
		errmsg << "Abs just needs at least 2 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosField::cloneJimo()  const
{
	try
	{
		return OmnNew AosField(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

