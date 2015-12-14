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
#include "Funcs/Jimos/ToChar.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosToChar_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosToChar(version);
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


AosToChar::AosToChar(const int version)
:
AosGenFunc("ToChar", version)
{
}

AosToChar::AosToChar()
:
AosGenFunc("ToChar", 1)
{
}



AosToChar::AosToChar(const AosToChar &rhs)
:
AosGenFunc(rhs)
{
}


AosToChar::~AosToChar()
{
}


bool
AosToChar::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosToChar::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}
	

bool
AosToChar::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	AosValueRslt vv1, vv2;
	if (!mParms[0]->getValue(rdata, record, vv1)) return false;
	if (vv1.isNull())
	{
		value.setNull();
		return true;
	}

	if (mSize >= 2)
	{
		if (!mParms[1]->getValue(rdata, record, vv2)) return false;
		if (vv2.isNull())
		{
			value = vv1;
			return true;
		}
	} 
	if (vv1.getType() == AosDataType::eDateTime)   
	{                                              
		OmnString data_format = vv2.getStr();
		AosDateTime dt = vv1.getDateTime();   
		OmnString str = dt.toString(data_format);  
		value.setStr(str);                       
	}                                              
	else
	{
		value.setNull();
	}
	return true;
}

bool 
AosToChar::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 2)
	{
		errmsg << "ToChar needs at 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosToChar::cloneJimo()  const
{
	try
	{
		return OmnNew AosToChar(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

