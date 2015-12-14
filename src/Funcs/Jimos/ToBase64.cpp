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
#include "Funcs/Jimos/ToBase64.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "Base64/Base64.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosToBase64_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosToBase64(version);
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


AosToBase64::AosToBase64(const int version)
:
AosGenFunc("ToBase64", version)
{
}

AosToBase64::AosToBase64()
:
AosGenFunc("ToBase64", 1)
{
}



AosToBase64::AosToBase64(const AosToBase64 &rhs)
:
AosGenFunc(rhs)
{
}


AosToBase64::~AosToBase64()
{
}


bool
AosToBase64::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosToBase64::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}

	
bool
AosToBase64::getValue(
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
	OmnString ostrToEncode = mValue.getStr();
	if(0 >= ostrToEncode.length())
	{
		OmnString val = "NULL";
		value.setStr(val);
		return true;
	}
	else
	{
		unsigned char* caEncodeBuf = OmnNew unsigned char[Aos_Funcs_Jimos_ToBase64_Max_Encoded_Len];
		if(!caEncodeBuf)
		{
			return false;
		}
		//it returns the encoded length which is not needed, so I just ignored it.
		EncodeBase64((unsigned char*)ostrToEncode.getBuffer(), caEncodeBuf, ostrToEncode.length(), Aos_Funcs_Jimos_ToBase64_Max_Encoded_Len);
		OmnString ostrEncoded((const char*)caEncodeBuf);
		value.setStr( ostrEncoded );
		OmnDelete[] caEncodeBuf;
		return true;
	}
}

bool 
AosToBase64::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "ToBase64 just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosToBase64::cloneJimo()  const
{
	try
	{
		return OmnNew AosToBase64(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

