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
#include "Funcs/Jimos/Convert.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time.hpp"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
using namespace boost::gregorian;
using namespace boost::posix_time;


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosConvert_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosConvert(version);
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


AosConvert::AosConvert(const int version)
:
AosGenFunc("Convert", version)
{
}

AosConvert::AosConvert()
:
AosGenFunc("Convert", 1)
{
}



AosConvert::AosConvert(const AosConvert &rhs)
:
AosGenFunc(rhs)
{
}


AosConvert::~AosConvert()
{
}


bool
AosConvert::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosConvert::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosConvert::getValue(
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
	string time_str = mValue.getStr();
	if (time_str == "")
	{
		value.setNull();
		return true;
	}
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	string from=mValue.getStr();
	if (from == "")
	{
		value.setNull();
		return true;
	}
	if (!AosGenFunc::getValue(rdata, 2, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	string to=mValue.getStr();
	if (to == "")
	{
		value.setNull();
		return true;
	}
	//
	boost::local_time::local_time_input_facet *input_facet =new boost::local_time::local_time_input_facet();
	istringstream is(time_str.c_str());
	is.imbue(std::locale(std::cout.getloc(), input_facet));
	input_facet->format(from.c_str());
	ptime p1;
	is >> p1;
	//OmnScreen<<"time_str is "<<p1<<endl;

	
	//p1=time_from_string(time_str);
	date d1=p1.date();
    date_facet * dfacet2= new date_facet(to.c_str());//("%Y年 %m月 %d日");
	cout.imbue(locale(cout.getloc(),dfacet2));
	cout<<"now is "<<d1<<endl;
	cout<<"p1  is "<<p1<<endl;
	ostringstream os;
	os.imbue(std::locale(std::cout.getloc(), dfacet2));
	os<<p1;
	OmnScreen<<os.str()<<endl;
    //
	value.setStr(os.str());
	return true;
}

bool 
AosConvert::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize!=3)
	{
		errmsg << "Convert only needs  3 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosConvert::cloneJimo()  const
{
	try
	{
		return OmnNew AosConvert(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

