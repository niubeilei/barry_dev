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
#include "Funcs/Jimos/Bit_count.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBit_count_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBit_count(version);
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


AosBit_count::AosBit_count(const int version)
:
AosGenFunc("Bit_count", version)
{
}

AosBit_count::AosBit_count()
:
AosGenFunc("Bit_count", 1)
{
}



AosBit_count::AosBit_count(const AosBit_count &rhs)
:
AosGenFunc(rhs)
{
}


AosBit_count::~AosBit_count()
{
}


bool
AosBit_count::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosBit_count::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eInt32;
}


bool
AosBit_count::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d = 0;
	AosValueRslt vv;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	d = mValue.getDouble();
	bitset<32> bs(d);
	string s=bs.to_string();
	int n=s.size();
	int k=0;
	for(int i=0;i<n;i++)
	{
		if(s[i]=='1')
		{
			k++;
		}
	}
	value.setI64(k);
	return true;
}


bool 
AosBit_count::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Abin need 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosBit_count::cloneJimo()  const
{
	try
	{
		return OmnNew AosBit_count(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

