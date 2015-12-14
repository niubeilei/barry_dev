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
#include "Funcs/Jimos/Asin.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAsin_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAsin(version);
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


AosAsin::AosAsin(const int version)
:
AosGenFunc("Asin", version)
{
}

AosAsin::AosAsin()
:
AosGenFunc("Asin", 1)
{
}



AosAsin::AosAsin(const AosAsin &rhs)
:
AosGenFunc(rhs)
{
}


AosAsin::~AosAsin()
{
}


bool
AosAsin::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosAsin::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosAsin::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d = 0;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	d = mValue.getDouble();
	if (d >= -1 && d <= 1)
	{
		value.setDouble( asin(d) );
	}
	else
	{
		//JimoDB_3_1 JIMODB-68 (Levi, 2015/07/22)
		OmnString msg = "";
		msg = "[ERR] : input value needs to be in the range [-1, 1].";
		rdata->setJqlMsg(msg);
	}

	return true;
}

bool 
AosAsin::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Asin need 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosAsin::cloneJimo()  const
{
	try
	{
		return OmnNew AosAsin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

