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
#include "Funcs/Jimos/ToNumber.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosToNumber_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosToNumber(version);
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


AosToNumber::AosToNumber(const int version)
:
AosGenFunc("ToChar", version)
{
}

AosToNumber::AosToNumber()
:
AosGenFunc("ToChar", 1)
{
}



AosToNumber::AosToNumber(const AosToNumber &rhs)
:
AosGenFunc(rhs)
{
}


AosToNumber::~AosToNumber()
{
}


bool
AosToNumber::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosToNumber::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eNumber;
}


bool
AosToNumber::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if(mSize == 1)
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		if (mValue.isNull())
		{
			value.setNull();
			return true;
		}
		OmnString data_format = mValue.getStr();
		int data_len = data_format.length();
		int idx = data_format.indexOf(".", 0);
		if(idx > 0)
		{
			OmnString start = data_format.substr(0, idx-1);
			OmnString end = data_format.substr(idx+1, data_len);

			OmnString zerostr;
			string ss(data_format.data());
			size_t found = ss.find_first_not_of("0");
			if (found!=string::npos)
			{
				zerostr = end.substr(0, found);	
			}
		
			OmnString val = "";
			val << start.toInt64(0)
				<< "."
				<< zerostr
				<< end.toInt64(0);
			
			value.setStr(val);
		}
		else
			value.setI64(data_format.toInt64(0));

	}
	else
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		if (mValue.isNull())
		{
			value.setNull();
			return true;
		}
		OmnString data_str = mValue.getStr();
		
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		if (mValue.isNull())
		{
			value.setNull();
			return true;
		}
		OmnString data_format = mValue.getStr();


		char str[100];
		//char* data_s = const_cast<char*>(data_str.data());
		sprintf(str, "%x", data_str.toInt64(0));
		value.setStr(OmnString(str, strlen(str)));

	}
	return true;
}

bool 
AosToNumber::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	
	if (mSize > 2)
	{
		errmsg << "ToChar needs at 1 parameter.";
		return false;
	}
	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosToNumber::cloneJimo()  const
{
	try
	{
		return OmnNew AosToNumber(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

