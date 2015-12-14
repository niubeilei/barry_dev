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
#include "Funcs/Jimos/Bin.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBin_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBin(version);
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


AosBin::AosBin(const int version)
:
AosGenFunc("Bin", version)
{
}

AosBin::AosBin()
:
AosGenFunc("Bin", 1)
{
}



AosBin::AosBin(const AosBin &rhs)
:
AosGenFunc(rhs)
{
}


AosBin::~AosBin()
{
}


bool
AosBin::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E
AosBin::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosBin::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d = 0;
	bool isField = false;
	AosExprObjPtr expr = mParms[0];
	OmnString name;

	//fix bug JIMODB-75, Gavin 2015/07/23
	aos_assert_r(expr, false);
	name = expr->dumpByNoEscape();
	isField = (expr->getType() == AosExprType::eFieldName); 
	if (isField && (!record || record->getFieldIdx(name, rdata) < 0))
	{
		OmnString errmsg;
		errmsg <<  "[ERR] : Unknown column " << name << " in 'field list' ,syntax error";
		rdata->setJqlMsg(errmsg);
		value.setNull();
		return true;
	}

	if (!AosGenFunc::getValue(rdata, 0, record)) return false;

	if (AosDataType::isNumericType(mValue.getType()) && mValue.getDouble())
	{
		OmnString errmsg;
		errmsg <<  "[ERR] : Unknown column " << name << " in 'field list' ,syntax error";
		rdata->setJqlMsg(errmsg);
		value.setNull();
		return true;

	}
	if (!AosDataType::isNumericType(mValue.getType()))
	{
		value.setChar('0');
		return true;
	}

	d = mValue.getDouble();
	bitset<32> bs(d);
	string s=bs.to_string( );

	int i,len ;
	char * ss =(char *)s.data();
	len = strlen(ss) ;
	for( i=0; i<len; i++)
	{
		if( ss[i] != '0') break ;
	}
	memmove(ss ,ss+i ,len-i+1);
	string v(ss);
	value.setStr(v);

	return true;
}


bool 
AosBin::syntaxCheck(
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
AosBin::cloneJimo()  const
{
	try
	{
		return OmnNew AosBin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

