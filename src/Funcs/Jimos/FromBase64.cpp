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
// 2014-11-4 Created by White
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/FromBase64.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "Base64/Base64.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFromBase64_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFromBase64(version);
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


AosFromBase64::AosFromBase64(const int version)
:
AosGenFunc("FromBase64", version)
{
}

AosFromBase64::AosFromBase64()
:
AosGenFunc("FromBase64", 1)
{
}



AosFromBase64::AosFromBase64(const AosFromBase64 &rhs)
:
AosGenFunc(rhs)
{
}


AosFromBase64::~AosFromBase64()
{
}


bool
AosFromBase64::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosFromBase64::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosFromBase64::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if(mValue.isNull())
	{
		OmnString val = "NULL";
		value.setStr(val);
		return true;	
	}
	OmnString ostrToDecode = mValue.getStr();
	if(0 >= ostrToDecode.length())
	{
		OmnString val = "NULL";
		value.setStr(val);
		return true;
	}
	else
	{
		unsigned char* caDecodeBuf = OmnNew unsigned char[Aos_Funcs_Jimos_FromBase64_Max_Encoded_Len];
		if(!caDecodeBuf)
		{
			return false;
		}
		//it returns the decoded length which is not needed, so I just ignored it.
		DecodeBase64((unsigned char*)ostrToDecode.getBuffer(), caDecodeBuf, ostrToDecode.length());
		OmnString ostrDecoded((const char*)caDecodeBuf);
		value.setStr( ostrDecoded );
		OmnDelete[] caDecodeBuf;
		return true;
	}
}

bool 
AosFromBase64::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "FromBase64 just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosFromBase64::cloneJimo()  const
{
	try
	{
		return OmnNew AosFromBase64(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
