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
// 2015/03/04 Created by Crystal Cao
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/CurrVal.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/SequenceMgrObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCurrVal_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCurrVal(version);
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


AosCurrVal::AosCurrVal(const int version)
:
AosGenFunc("CurrVal", version)
{
}

AosCurrVal::AosCurrVal()
:
AosGenFunc("CurrVal", 1)
{
}



AosCurrVal::AosCurrVal(const AosCurrVal &rhs)
:
AosGenFunc(rhs)
{
}


AosCurrVal::~AosCurrVal()
{
}


bool
AosCurrVal::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCurrVal::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosCurrVal::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString seqName = mValue.getStr();

	bool isExist = AosCheckSequenceExist(rdata, seqName, "currval");
	aos_assert_r(isExist, false);
	if (isExist)
	{
		int reVal = 0;
		bool rslt = AosSequenceMgrObj::getSequenceMgr()->getCrtValue(rdata, seqName, reVal);
		if (rslt)
		{
			value.setI64(reVal);
		}
	} 
	return true;
}

bool 
AosCurrVal::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "CurrVal just needs 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosCurrVal::cloneJimo()  const
{
	try
	{
		return OmnNew AosCurrVal(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

